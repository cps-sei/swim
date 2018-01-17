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
#include "SwimClient.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>

using boost::asio::ip::tcp;
using namespace std;

SwimClient::SwimClient() : socket(ioService) {
}

void SwimClient::connect(const char* host, const char* service) {
    tcp::resolver resolver(ioService);

    tcp::resolver::query query(host, service);
    tcp::resolver::iterator endpointIt = resolver.resolve(query);
    tcp::resolver::iterator end;

    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpointIt != end)
    {
      socket.close();
      socket.connect(*endpointIt++, error);
    }
    if (error) {
        socket.close();
        throw boost::system::system_error(error);
    }
}

bool SwimClient::isConnected() const {
    return socket.is_open();
}

std::string SwimClient::sendCommand(const char* command) {
    if (!socket.is_open()) {
        throw std::runtime_error("socket is closed");
    }

    boost::system::error_code ignored_error;
    boost::asio::write(socket, boost::asio::buffer(command, strlen(command)),
              boost::asio::transfer_all(), ignored_error);


    vector<char> buffer(128);
    boost::system::error_code error;

    size_t len = socket.read_some(boost::asio::buffer(buffer), error);

    if (error)
        throw boost::system::system_error(error);

    string reply(buffer.data(), len);
    if (reply.compare(0, 6, "error:") == 0) {
        // trim strings
        size_t last = reply.length() - 1;
        while (isspace(reply.at(last))) {
            reply.erase(last--);
        }
        string cmd(command);
        last = cmd.length() - 1;
        while (isspace(cmd.at(last))) {
            cmd.erase(last--);
        }

        throw std::logic_error(reply + "(cmd " + cmd + ')');
    }

    return reply;
}

double SwimClient::probeDouble(const char* command) {
    string reply = sendCommand(command);
    return atof(reply.c_str());
}

int SwimClient::probeInt(const char* command) {
    string reply = sendCommand(command);
    return atoi(reply.c_str());
}

double SwimClient::getDimmer() {
    return probeDouble("get_dimmer\n");
}

int SwimClient::getServers() {
    return probeInt("get_servers\n");
}

int SwimClient::getActiveServers() {
    return probeInt("get_active_servers\n");
}

int SwimClient::getMaxServers() {
    return probeInt("get_max_servers\n");
}

double SwimClient::getUtilization(int serverId) {
    ostringstream cmd;
    cmd << "get_utilization server" << serverId << '\n';
    return probeDouble(cmd.str().c_str());
}

double SwimClient::getBasicResponseTime() {
    return probeDouble("get_basic_rt\n");
}

double SwimClient::getOptionalResponseTime() {
    return probeDouble("get_opt_rt\n");
}

double SwimClient::getBasicThroughput() {
    return probeDouble("get_basic_throughput\n");
}

double SwimClient::getOptionalThroughput() {
    return probeDouble("get_opt_throughput\n");
}

double SwimClient::getArrivalRate() {
    return probeDouble("get_arrival_rate\n");
}

void SwimClient::addServer() {
    sendCommand("add_server\n");
}

void SwimClient::removeServer() {
    sendCommand("remove_server\n");
}

void SwimClient::setDimmer(double dimmer) {
    ostringstream cmd;
    cmd << "set_dimmer " << dimmer << '\n';
    probeDouble(cmd.str().c_str());
}

SwimClient::~SwimClient() {
    socket.close();
}

double SwimClient::getTotalUtilization() {
    double utilization = 0;
    int activeServers = getActiveServers();
    for (int s = 1; s <= activeServers; s++) {
        utilization += getUtilization(s);
    }
    return utilization;
}

double SwimClient::getAverageResponseTime() {
    double basicTput = getBasicThroughput();
    double optTput = getOptionalThroughput();

    double avgResponseTime = basicTput * getBasicResponseTime()
            + optTput * getOptionalResponseTime() / (basicTput + optTput);

    return avgResponseTime;
}
