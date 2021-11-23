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
#include "SimProbe.h"
#include <model/Model.h>
#include <managers/execution/ExecutionManagerModBase.h>

using namespace omnetpp;

Define_Module(SimProbe);

void SimProbe::initialize(int stage)
{
    if (stage == 1) {
        lifeTimeSignal = registerSignal("lifeTime");
        getSimulation()->getSystemModule()->subscribe(lifeTimeSignal, this);

        serverBusySignal = registerSignal("busy");
        getSimulation()->getSystemModule()->subscribe(serverBusySignal, this);

        interArrivalSignal = registerSignal("interArrival");
        getSimulation()->getSystemModule()->subscribe(interArrivalSignal, this);

        serverRemovedSignal = registerSignal(ExecutionManagerModBase::SIG_SERVER_REMOVED);
        getSimulation()->getSystemModule()->subscribe(serverRemovedSignal, this);

        Model* pModel = check_and_cast<Model*>(
                        getParentModule()->getSubmodule("model"));
        window = pModel->getEvaluationPeriod();
        arrival.setWindow(window);
        basicResponseTime.setWindow(window);
        optResponseTime.setWindow(window);
    }
}

double SimProbe::getBasicResponseTime() {
    return basicResponseTime.getAverage();
}

double SimProbe::getOptResponseTime() {
    return optResponseTime.getAverage();
}

double SimProbe::getBasicThroughput() {
    return basicResponseTime.getRate();
}

double SimProbe::getOptThroughput() {
    return optResponseTime.getRate();
}

double SimProbe::getUtilization(const std::string& serverName) {
    auto it = utilization.find(serverName);
    if (it != utilization.end()) {
        return it->second.getPercentageAboveZero();
    }

    return -1.0; // error: server not found
}

double SimProbe::getArrivalRate() {
    return arrival.getRate();
}

void SimProbe::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void SimProbe::receiveSignal(cComponent *source, simsignal_t signalID,
        const SimTime& t, cObject *details) {
    if (signalID == lifeTimeSignal) {
        bool basicService = (strcmp(source->getName(), "sinkLow") == 0);
        if (basicService) {
            basicResponseTime.record(t.dbl());
        } else {
            optResponseTime.record(t.dbl());
       }
    }
}


void SimProbe::receiveSignal(cComponent *source, simsignal_t signalID,
        bool value, cObject *details) {
    if (signalID == serverBusySignal) {
        std::string serverName = source->getParentModule()->getName(); // because it is nested
        if (serverName[0] == 'R') {
            // it is a removed server, skip it
            return;
        }
        auto it = utilization.find(serverName);
        if (it != utilization.end()) {
            it->second.record((value) ? 1.0 : 0.0); // busy vs idle
        } else {

            /*
             * a new server emits a busy=false signal when it is initialized.
             * It should not be recorded for a new server because it then
             * throws off the sliding window avg utilization computation
             */
            if (value) {
                auto& util = utilization[serverName];
                util.setWindow(window);
                util.record(1.0); // busy
            }
        }
    }
}

void SimProbe::receiveSignal(cComponent *source, simsignal_t signalID,
        double value, cObject *details) {
    if (signalID == interArrivalSignal) {
        arrival.record(value);
    }
}

Observations SimProbe::getUpdatedObservations() {
    Observations obs;
    obs.utilization = 0;

    for (auto& entry : utilization) {
        obs.utilization += entry.second.getPercentageAboveZero();
    }

    obs.basicResponseTime = getBasicResponseTime();
    obs.basicThroughput = getBasicThroughput();
    obs.optResponseTime = getOptResponseTime();
    obs.optThroughput = getOptThroughput();
    obs.avgResponseTime = (obs.basicResponseTime * obs.basicThroughput + obs.optResponseTime * obs.optThroughput)
            / (obs.basicThroughput + obs.optThroughput);

    return obs;
}

Environment SimProbe::getUpdatedEnvironment() {
    double arrivalRate = getArrivalRate();
    double measuredMeanInterArrival = (arrivalRate > 0) ? (1 / arrivalRate) : 0;

    Environment environment;
    environment.setArrivalMean(measuredMeanInterArrival);
    environment.setArrivalVariance(pow(measuredMeanInterArrival, 2)); // assume exponential distribution
    return environment;
}


void SimProbe::receiveSignal(cComponent *source, simsignal_t signalID, const char* value, cObject *details) {
    if (signalID == serverRemovedSignal) {
        auto it = utilization.find(value);
        if (it != utilization.end()) {
            utilization.erase(it);
        }
    }
}
