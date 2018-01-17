/*******************************************************************************
 * Simulator of Web Infrastructure and Management
 * Copyright (c) 2016 Carnegie Mellon University.
 * All Rights Reserved.
 *  
 * THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
 * MELLON UNIVERSITY EXPRESSLY DISCLAIMS TO THE FULLEST EXTENT PERMITTED BY LAW
 * ALL EXPRESS, IMPLIED, AND STATUTORY WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
 *  
 * Released under a BSD license, please see license.txt for full terms.
 * DM-0003883
 *******************************************************************************/
#include "AdaptInterface.h"
#include <string>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <managers/execution/ExecutionManagerMod.h>

Define_Module(AdaptInterface);

#define DEBUG_ADAPT_INTERFACE 0

using namespace std;

namespace {
    const string UNKNOWN_COMMAND = "error: unknown command\n";
    const string COMMAND_SUCCESS = "OK\n";
}


AdaptInterface::AdaptInterface() {
    commandHandlers["add_server"] = std::bind(&AdaptInterface::cmdAddServer, this, std::placeholders::_1);
    commandHandlers["remove_server"] = std::bind(&AdaptInterface::cmdRemoveServer, this, std::placeholders::_1);
    commandHandlers["set_dimmer"] = std::bind(&AdaptInterface::cmdSetDimmer, this, std::placeholders::_1);


    // get commands
    commandHandlers["get_dimmer"] = std::bind(&AdaptInterface::cmdGetDimmer, this, std::placeholders::_1);
    commandHandlers["get_servers"] = std::bind(&AdaptInterface::cmdGetServers, this, std::placeholders::_1);
    commandHandlers["get_active_servers"] = std::bind(&AdaptInterface::cmdGetActiveServers, this, std::placeholders::_1);
    commandHandlers["get_max_servers"] = std::bind(&AdaptInterface::cmdGetMaxServers, this, std::placeholders::_1);
    commandHandlers["get_utilization"] = std::bind(&AdaptInterface::cmdGetUtilization, this, std::placeholders::_1);
    commandHandlers["get_basic_rt"] = std::bind(&AdaptInterface::cmdGetBasicResponseTime, this, std::placeholders::_1);
    commandHandlers["get_basic_throughput"] = std::bind(&AdaptInterface::cmdGetBasicThroughput, this, std::placeholders::_1);
    commandHandlers["get_opt_rt"] = std::bind(&AdaptInterface::cmdGetOptResponseTime, this, std::placeholders::_1);
    commandHandlers["get_opt_throughput"] = std::bind(&AdaptInterface::cmdGetOptThroughput, this, std::placeholders::_1);
    commandHandlers["get_arrival_rate"] = std::bind(&AdaptInterface::cmdGetArrivalRate, this, std::placeholders::_1);

    // dimmer, numServers, numActiveServers, utilization(total or indiv), response time and throughput for mandatory and optional, avg arrival rate
}

AdaptInterface::~AdaptInterface() {
    cancelAndDelete(rtEvent);
}

void AdaptInterface::initialize()
{
    rtEvent = new cMessage("rtEvent");
    rtScheduler = check_and_cast<cSocketRTScheduler *>(getSimulation()->getScheduler());
    rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, BUFFER_SIZE, &numRecvBytes);
    pModel = check_and_cast<Model*> (getParentModule()->getSubmodule("model"));
    pProbe = check_and_cast<IProbe*> (gate("probe")->getPreviousGate()->getOwnerModule());
}

void AdaptInterface::handleMessage(cMessage *msg)
{
    if (msg == rtEvent) {

        // get data from buffer
        string input = string(recvBuffer, numRecvBytes);
        numRecvBytes = 0;
#if DEBUG_ADAPT_INTERFACE
        EV << "received [" << input << "]" << endl;
#endif
        std::istringstream inputStream(input);

        std::string line;
        while (std::getline(inputStream, line))
        {
            line.erase(line.find_last_not_of("\r\n") + 1);
#if DEBUG_ADAPT_INTERFACE
            cout << "received line is [" << line << "]" << endl;
#endif
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            tokenizer tokens(line, boost::char_separator<char>(" "));
            tokenizer::iterator it = tokens.begin();
            if (it != tokens.end()) {
                string command = *it;
                vector<string> args;
                while (++it != tokens.end()) {
                    args.push_back(*it);
                }

                auto handler = commandHandlers.find(command);
                if (handler == commandHandlers.end()) {
                    rtScheduler->sendBytes(UNKNOWN_COMMAND.c_str(), UNKNOWN_COMMAND.length());
                } else {
                    string reply = commandHandlers[command](args);
#if DEBUG_ADAPT_INTERFACE
                    cout << "command reply is[" << reply << ']' << endl;
#endif
                    rtScheduler->sendBytes(reply.c_str(), reply.length());
                }
            }

        }

    }
}

std::string AdaptInterface::cmdAddServer(const std::vector<std::string>& args) {
    ExecutionManagerModBase* pExecMgr = check_and_cast<ExecutionManagerModBase*> (getParentModule()->getSubmodule("executionManager"));
    pExecMgr->addServer();

    return COMMAND_SUCCESS;
}

std::string AdaptInterface::cmdRemoveServer(const std::vector<std::string>& args) {
    ExecutionManagerModBase* pExecMgr = check_and_cast<ExecutionManagerModBase*> (getParentModule()->getSubmodule("executionManager"));
    pExecMgr->removeServer();

    return COMMAND_SUCCESS;
}

std::string AdaptInterface::cmdSetDimmer(const std::vector<std::string>& args) {
    if (args.size() == 0) {
        return "error: missing dimmer argument\n";
    }

    double dimmer = atof(args[0].c_str());
    ExecutionManagerModBase* pExecMgr = check_and_cast<ExecutionManagerModBase*> (getParentModule()->getSubmodule("executionManager"));
    pExecMgr->setBrownout(1 - dimmer);

    return COMMAND_SUCCESS;
}


std::string AdaptInterface::cmdGetDimmer(const std::vector<std::string>& args) {
    ostringstream reply;
    double brownoutFactor = pModel->getBrownoutFactor();
    double dimmer = (1 - brownoutFactor);

    reply << dimmer << '\n';

    return reply.str();
}


std::string AdaptInterface::cmdGetServers(const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pModel->getServers() << '\n';

    return reply.str();
}


std::string AdaptInterface::cmdGetActiveServers(const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pModel->getActiveServers() << '\n';

    return reply.str();
}


std::string AdaptInterface::cmdGetMaxServers(const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pModel->getMaxServers() << '\n';

    return reply.str();
}


std::string AdaptInterface::cmdGetUtilization(const std::vector<std::string>& args) {
    if (args.size() == 0) {
        return "error: missing server argument\n";
    }

    ostringstream reply;
    auto utilization = pProbe->getUtilization(args[0]);
    if (utilization < 0) {
        reply << "error: server \'" << args[0] << "\' does no exist";
    } else {
        reply << utilization;
    }
    reply << '\n';

    return reply.str();
}

std::string AdaptInterface::cmdGetBasicResponseTime(
        const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pProbe->getBasicResponseTime() << '\n';

    return reply.str();
}

std::string AdaptInterface::cmdGetBasicThroughput(
        const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pProbe->getBasicThroughput() << '\n';

    return reply.str();
}

std::string AdaptInterface::cmdGetOptResponseTime(
        const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pProbe->getOptResponseTime() << '\n';

    return reply.str();
}

std::string AdaptInterface::cmdGetOptThroughput(
        const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pProbe->getOptThroughput() << '\n';

    return reply.str();
}

std::string AdaptInterface::cmdGetArrivalRate(
        const std::vector<std::string>& args) {
    ostringstream reply;
    reply << pProbe->getArrivalRate() << '\n';

    return reply.str();
}
