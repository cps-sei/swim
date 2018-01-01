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
#include "SimpleMonitor.h"

#include <cmath>
#include "managers/adaptation/UtilityScorer.h"
#include "managers/ModulePriorities.h"
#include "managers/execution/ExecutionManagerModBase.h"

using namespace std;
using namespace omnetpp;

Define_Module(SimpleMonitor);

SimpleMonitor::SimpleMonitor()
{
    periodEvent = 0;
    periodPostEvent = 0;
}

void SimpleMonitor::initialize(int stage) {
    if (stage == 1) {
        numberOfServersSignal = registerSignal("numberOfServers");
        activeServersSignal = registerSignal("activeServers");
        measuredInterarrivalAvg = registerSignal("measuredInterarrivalAvg");
        measuredInterarrivalStdDev = registerSignal("measuredInterarrivalStdDev");
        utilitySignal = registerSignal("utility");
        brownoutFactorSignal = registerSignal("brownoutFactor");

        serverRemovedSignal = registerSignal(ExecutionManagerModBase::SIG_SERVER_REMOVED);
        serverAddedSignal = registerSignal(ExecutionManagerModBase::SIG_SERVER_ADDED);
        serverActivatedSignal = registerSignal(ExecutionManagerModBase::SIG_SERVER_ACTIVATED);
        brownoutSetSignal = registerSignal(ExecutionManagerModBase::SIG_BROWNOUT_SET);

        oversamplingFactor = par("oversamplingFactor");

        EV << "subscribing to signals" << endl;
        getSimulation()->getSystemModule()->subscribe(serverRemovedSignal, this);
        getSimulation()->getSystemModule()->subscribe(serverAddedSignal, this);
        getSimulation()->getSystemModule()->subscribe(serverActivatedSignal, this);
        getSimulation()->getSystemModule()->subscribe(brownoutSetSignal, this);

        pModel = check_and_cast<Model*>(
                getParentModule()->getSubmodule("model"));
        pProbe = check_and_cast<IProbe*> (gate("probe")->getPreviousGate()->getOwnerModule());

        // Create the event objects we'll use for timing -- just any ordinary message.
        periodEvent = new cMessage("periodEvent");
        periodEvent->setSchedulingPriority(MONITOR_PRE_PRIO);
        scheduleAt(pModel->getEvaluationPeriod(), periodEvent);

        periodPostEvent = new cMessage("periodPostEvent");
        periodPostEvent->setSchedulingPriority(MONITOR_POST_PRIO);
        scheduleAt(0, periodPostEvent);

        oversamplingEvent = new cMessage("oversamplingEvent");
        oversamplingEvent->setSchedulingPriority(MONITOR_PRE_OVERSAMPLING_PRIO);
        scheduleAt(computeNextEventTime(oversamplingFactor), oversamplingEvent);
    }
}

/**
 * Computes the time for the next oversampling event
 */
simtime_t SimpleMonitor::computeNextEventTime(unsigned factor) const {
    simtime_t t = simTime() + pModel->getEvaluationPeriod() / factor;

    // if it's close to the evaluation period, make it equal so that they don't get out of sync
    if (abs((t - periodEvent->getArrivalTime()).dbl()) < 0.01) {
        t = periodEvent->getArrivalTime();
    }

    return t;
}

void SimpleMonitor::handleMessage(cMessage *msg) {

    /*
     * the monitoring is divided into three parts. One handles events at rates faster than
     * the evaluation period.
     * Then, one that executes before the adaptation manager
     * and the other that executes after, so that statistics signals can reflect what the
     * adaptation manager just did (like adding a server)
     */
    if (msg == oversamplingEvent) {
        oversamplingHandler();
        scheduleAt(computeNextEventTime(oversamplingFactor), oversamplingEvent);
    } else if (msg == periodEvent) {
        periodicHandler();
        scheduleAt(simTime() + pModel->getEvaluationPeriod(), periodEvent);
    } else if (msg == periodPostEvent) {
        postPeriodHandler();
        scheduleAt(simTime() + pModel->getEvaluationPeriod(), periodPostEvent);
    }
}

void SimpleMonitor::periodicHandler() {
    Observations observations = pProbe->getUpdatedObservations();
    pModel->setObservations(observations);

    const Environment& env = pModel->getEnvironment();
    emit(measuredInterarrivalAvg, env.getArrivalMean());
    emit(measuredInterarrivalStdDev, sqrt(env.getArrivalVariance()));

    double periodUtility = UtilityScorer::getAccruedUtility(*pModel,
            pModel->getConfiguration(), pModel->getEnvironment(), pModel->getObservations())
        * pModel->getEvaluationPeriod();
    emit(utilitySignal, periodUtility);
}

void SimpleMonitor::oversamplingHandler() {
    Environment environment = pProbe->getUpdatedEnvironment();
    pModel->setEnvironment(environment);
}

void SimpleMonitor::postPeriodHandler() {
    emit(numberOfServersSignal, pModel->getServers());
    emit(activeServersSignal,
            pModel->getActiveServers());
    emit(brownoutFactorSignal,
            pModel->getBrownoutFactor());
}


void SimpleMonitor::receiveSignal(cComponent *source, simsignal_t signalID,
        bool value, cObject *details) {
    if (signalID == serverActivatedSignal) {
        emit(activeServersSignal,
                pModel->getActiveServers());
    } else if (signalID == serverAddedSignal) {
        emit(numberOfServersSignal, pModel->getServers());
    } else if (signalID == brownoutSetSignal) {
        emit(brownoutFactorSignal,
                pModel->getBrownoutFactor());
    }
}


void SimpleMonitor::receiveSignal(cComponent *source, simsignal_t signalID,
        const char* value, cObject *details) {
    if (signalID == serverRemovedSignal) {
        emit(numberOfServersSignal, pModel->getServers());
        emit(activeServersSignal,
                pModel->getActiveServers());

    }
}


SimpleMonitor::~SimpleMonitor() {
    cancelAndDelete(periodEvent);
    cancelAndDelete(periodPostEvent);
    cancelAndDelete(oversamplingEvent);
}

