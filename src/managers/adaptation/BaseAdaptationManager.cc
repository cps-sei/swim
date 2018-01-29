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

#include <managers/adaptation/BaseAdaptationManager.h>
#include "managers/ModulePriorities.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>

using namespace boost::posix_time;
using namespace omnetpp;

BaseAdaptationManager::BaseAdaptationManager() :
        periodEvent(nullptr), decisionCompleteEvent(nullptr),
        pTactic(nullptr) {}

void BaseAdaptationManager::initialize(int stage)
{
    if (stage == 0) {
        pModel = check_and_cast<Model*> (getParentModule()->getSubmodule("model"));
        decisionTimeSignal = registerSignal("decisionTime");
    } else {

        // Create the event object we'll use for timing -- just any ordinary message.
        periodEvent = new cMessage("adaptMgrPeriod");
        periodEvent->setSchedulingPriority(ADAPTATION_MGR_PRIO);

        decisionCompleteEvent = new cMessage("adaptDecisionComplete");
        decisionCompleteEvent->setSchedulingPriority(ADAPTATION_MGR_PRIO);

        // start adaptation half way through the warmup period
        double adaptationWarmup = ceil(
                    0.5 * getSimulation()->getWarmupPeriod().dbl()
                            / pModel->getEvaluationPeriod())
                * pModel->getEvaluationPeriod();

        scheduleAt(adaptationWarmup + pModel->getEvaluationPeriod(), periodEvent);
    }
}

void BaseAdaptationManager::decisionComplete() {
    if (pTactic) {
        if (!pTactic->isEmpty()) {
            std::cout << "AdaptationMgr simtime=" << simTime() << " tactic=" << *pTactic << std::endl;
            ExecutionManager* pExecMgr = check_and_cast<ExecutionManager*> (getParentModule()->getSubmodule("executionManager"));
            pTactic->execute(pExecMgr);
        }
        delete pTactic;
        pTactic = nullptr;
    }
}

void BaseAdaptationManager::handleMessage(cMessage *msg)
{
    if (msg == periodEvent) {
        if (decisionCompleteEvent->isScheduled()) {
            std::cout << "AdaptationMgr simtime=" << simTime() << " skipping decision" << std::endl;
        } else {
            assert(pTactic == nullptr);
            ptime startTime = microsec_clock::local_time();
            pTactic = evaluate();
            double decisionTime = (double) (microsec_clock::local_time() - startTime).total_milliseconds();
            emit(decisionTimeSignal, decisionTime);

            if (par("simulateDecisionDelay")) {
                scheduleAt(simTime() + decisionTime / 1000, decisionCompleteEvent);
            } else {
                decisionComplete();
            }
        }
        scheduleAt(simTime() + pModel->getEvaluationPeriod(), periodEvent);
    } else if (msg == decisionCompleteEvent) {
        decisionComplete();
    }
}

BaseAdaptationManager::~BaseAdaptationManager() {
    cancelAndDelete(periodEvent);
    cancelAndDelete(decisionCompleteEvent);
    if (pTactic) delete(pTactic);
}
