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
#include "HAProxyProbe.h"
#include <sstream>
#include "managers/ModulePriorities.h"
#include <managers/execution/ExecutionManagerModBase.h>

using namespace std;
using namespace omnetpp;

Define_Module(HAProxyProbe);

Observations HAProxyProbe::getUpdatedObservations() {

    // update observations only if not stale
    auto currentTime = boost::posix_time::microsec_clock::local_time();
    if (!lastObservationsUpdate.is_not_a_date_time()
            && (currentTime - lastObservationsUpdate).total_milliseconds()
                    <= MEASUREMENT_OBSOLECENSE_MSEC) {
        return observations;
    }

    lastObservationsUpdate = currentTime;

    tcpStream << "stats" << endl;
    string response;
    getline(tcpStream, response);
    cout << "LogFileProbe stats response: " << response << endl;

    stringstream responseStream(response);
    string valueStr;
    getline(responseStream, valueStr, ',');
    // arrivalRate = stod(valueStr);
    getline(responseStream, valueStr, ',');
    ASSERT(valueStr == "2");
    getline(responseStream, valueStr, ',');
    observations.basicThroughput = stod(valueStr);
    getline(responseStream, valueStr, ',');
    observations.basicResponseTime = stod(valueStr) / 1000;
    getline(responseStream, valueStr, ',');
    observations.optThroughput = stod(valueStr);
    if (!getline(responseStream, valueStr, ',')) {
        error("response from LogFileProbe does not match expected format");
    }
    observations.optResponseTime = stod(valueStr) / 1000;

    observations.avgResponseTime = (observations.basicResponseTime * observations.basicThroughput + observations.optResponseTime * observations.optThroughput)
            / (observations.basicThroughput + observations.optThroughput);

    // get total utilization
    observations.utilization = 0;
    tcpStream << "util" << endl;
    getline(tcpStream, response);
    cout << "LogFileProbe util response: " << response << endl;

    responseStream.clear();
    responseStream.str(response);

    /*
     * only take the utilization of the active servers
     * This assumes that if there are c active servers, the first c servers
     * are the active ones
     */
    int activeServers = pModel->getActiveServers();
    int server = 1;
    utilization.clear();
    while (server <= activeServers && getline(responseStream, valueStr, ',')) {
        double serverUtilization = stod(valueStr) / 100.0;

        // record in utilization table
        stringstream serverId;
        serverId << "server";
        serverId << server;
        utilization[serverId.str()] = serverUtilization;

        // compute total utilization
        observations.utilization += serverUtilization;
        server++;
    }
    return observations;
}

Environment HAProxyProbe::getUpdatedEnvironment() {

    // update environment only if not stale
    auto currentTime = boost::posix_time::microsec_clock::local_time();
    if (lastEnvironmentUpdate.is_not_a_date_time()
            || (currentTime - lastEnvironmentUpdate).total_milliseconds()
                    > MEASUREMENT_OBSOLECENSE_MSEC) {
        lastEnvironmentUpdate = currentTime;
        double arrivalRate = getAverageRequestRate();
        double measuredMeanInterArrival = (arrivalRate > 0) ? (1 / arrivalRate) : 0;

        environment.setArrivalMean(measuredMeanInterArrival);
        environment.setArrivalVariance(pow(measuredMeanInterArrival, 2)); // assume exponential distribution
    }

    return environment;
}

void HAProxyProbe::handleMessage(cMessage* msg) {
    if (msg == initEvent) {
        /*
         * sending the sync signal to start playing trace here when using the realtime clock
         * causes the replaying to start later than at time 0 because the Alloy run
         * is started after time 0 baseline is set, so all the other events are they
         * compressed until the event queue catches up with the realtime clock.
         * That's why it was move to the initialization
         */
        //sendReplayTraceSyncSignal();
    } else if (msg == endWarmupEvent) {
        // get time
        tcpStream << "time" << endl;
        string response;
        getline(tcpStream, response);
        long epochTime = stol(response);
        emit(registerSignal("hapHostBaseTime"), epochTime);
        cout << "LogFileProbe time response: " << epochTime << endl;
    }
}

double HAProxyProbe::getBasicResponseTime() {
    return getUpdatedObservations().basicResponseTime;
}

double HAProxyProbe::getOptResponseTime() {
    return getUpdatedObservations().optResponseTime;
}

double HAProxyProbe::getBasicThroughput() {
    return getUpdatedObservations().basicThroughput;
}

double HAProxyProbe::getOptThroughput() {
    return getUpdatedObservations().optThroughput;
}

double HAProxyProbe::getUtilization(const std::string& serverName) {
    getUpdatedObservations();
    auto it = utilization.find(serverName);
    if (it != utilization.end()) {
        return it->second;
    }

    return -1.0; // error: server not found
}

double HAProxyProbe::getArrivalRate() {
    double arrivalMean = getUpdatedEnvironment().getArrivalMean();
    double rate = 0;
    if (arrivalMean > 0) {
        rate = 1.0 / arrivalMean;
    }
    return rate;
}

HAProxyProbe::~HAProxyProbe() {
    cancelAndDelete(initEvent);
    cancelAndDelete(endWarmupEvent);
}

void HAProxyProbe::sendReplayTraceSyncSignal() {
    // send sync signal to start playing trace
    string replayTracePort = par("replayTraceSyncPort");
    if (replayTracePort.length() > 0) {
        cout << "Connecting for replaytrace sync at " << replayTracePort
                << "...";
        boost::asio::ip::tcp::iostream tcpSyncStream;
        tcpSyncStream.connect("localhost", replayTracePort);
        if (!tcpSyncStream) {
            cout << "failed: " << tcpSyncStream.error().message() << endl;
        } else {
            tcpSyncStream.close();
            cout << "done" << endl;
        }
    }
}

void HAProxyProbe::initialize(int stage) {
    if (stage == 0) {
        // send sync signal to start playing trace
        sendReplayTraceSyncSignal();
    } else if (stage == 1) {
        serverRemovedSignal = registerSignal(ExecutionManagerModBase::SIG_SERVER_REMOVED);
        getSimulation()->getSystemModule()->subscribe(serverRemovedSignal, this);

        pModel = check_and_cast<Model*>(
                getParentModule()->getSubmodule("model"));

        loadBalancer.setAddress(par("HAProxySocketPath").stringValue());
        lastRequestCounter = 0;

        // connect to the logfileprobe
        cout << "Connecting to LogFileProbe...";
        tcpStream.connect("localhost", "5127");
        if (!tcpStream) {
          cout << "Error Unable to connect to LogFileProbe server: " << tcpStream.error().message() << endl;
          error("Error connecting to LogFileProbe");
        }
        cout << "done" << endl;

        // set evaluation period for probing
        tcpStream << "window " << (int) pModel->getEvaluationPeriod() << endl;

        // schedule event to happen at time 0 after all the initialization to trigger trace replay
        initEvent = new cMessage("initEvent");
        initEvent->setSchedulingPriority(MONITOR_PRE_PRIO);
        scheduleAt(0, initEvent);

        // schedule event to happen at time 0 after all the initialization to get HAP host time
        endWarmupEvent = new cMessage("endWarmupEvent");
        endWarmupEvent->setSchedulingPriority(MONITOR_PRE_PRIO);
        scheduleAt(getSimulation()->getWarmupPeriod(), endWarmupEvent);
    }
}

/*
 * HAProxy only reports avg request rate for the last second, so, to use this approach
 * we would need to collect the rate several times over the period to get an average for the period
 * This code is left here in case it is needed later, but it's not used now
 */
#if 0
void HAProxyProbe::sampleRequestRate() {
    string reply = loadBalancer.executeCommand("show stat -1 1 -1\n");

    // skip the header line
    size_t pos = reply.find_first_of('\n');

    // 46. req_rate [.F..]: HTTP requests per second over last elapsed second (columns start with idx 0)
    int column = 0;
    while (pos != string::npos && column < 46) {
        pos = reply.find_first_of(',', pos + 1);
        column++;
    }
    ASSERT(pos != string::npos);
    pos++;
    string value = reply.substr(pos, reply.find_first_of(',', pos) - pos);
    double rate = atof(value.c_str());
    //cout << "measured interarrival rate " << rate << " pos " << pos << endl << reply << endl;
    pRequestRateStats->operator ()(rate);
}
#endif

double HAProxyProbe::getAverageRequestRate() {
    boost::posix_time::ptime counterResetTime = boost::posix_time::microsec_clock::local_time();
    string reply = loadBalancer.executeCommand("show stat -1 1 -1\n");

    // skip the header line
    size_t pos = reply.find_first_of('\n');

    // 48. req_tot [.F..]: total number of HTTP requests received (columns start with idx 0)
    int column = 0;
    while (pos != string::npos && column < 48) {
        pos = reply.find_first_of(',', pos + 1);
        column++;
    }
    ASSERT(pos != string::npos);
    pos++;
    string value = reply.substr(pos, reply.find_first_of(',', pos) - pos);
    double requestCounter = atof(value.c_str());
    double avgRequestRate = 0;
    if (!lastCounterResetTime.is_not_a_date_time()) {
        avgRequestRate =
                (requestCounter - lastRequestCounter)
                        / ((counterResetTime - lastCounterResetTime).total_milliseconds()
                                / 1000.0);
    }
    lastRequestCounter = requestCounter;
    lastCounterResetTime = counterResetTime;

    //cout << "measured interarrival rate " << rate << " pos " << pos << endl << reply << endl;
    return avgRequestRate;
}

void HAProxyProbe::connectToLogFileProbe() {

}

void HAProxyProbe::receiveSignal(cComponent *source, simsignal_t signalID, const char* value, cObject *details) {
    if (signalID == serverRemovedSignal) {
        auto it = utilization.find(value);
        if (it != utilization.end()) {
            utilization.erase(it);
        }
    }
}
