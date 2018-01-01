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

#include "MTServer.h"
#include "Job.h"
#include "SelectionStrategies.h"
#include "IPassiveQueue.h"

Define_Module(MTServer);

using namespace queueing;

MTServer::MTServer() : endExecutionMsg(NULL), selectionStrategy(NULL), maxThreads(0) {}

void MTServer::initialize() {
    busySignal = registerSignal("busy");
    emit(busySignal, false);
    maxThreads = par("threads");
    endExecutionMsg = new cMessage("end-execution");
    selectionStrategy = SelectionStrategy::create(par("fetchingAlgorithm"), this, true);
    if (!selectionStrategy)
        error("invalid selection strategy");
    timeout = par("timeout");
}

MTServer::~MTServer() {
    cancelAndDelete(endExecutionMsg);
    delete selectionStrategy;
    for (RunningJobs::iterator it = runningJobs.begin(); it != runningJobs.end(); ++it) {
        delete it->pJob;
    }
}

void MTServer::handleMessage(cMessage* msg) {
    if (msg == endExecutionMsg)
    {
        ASSERT(!runningJobs.empty());
        updateJobTimes();

        // send out all jobs that completed
        RunningJobs::iterator first = runningJobs.begin();
        while (first != runningJobs.end() && first->remainingServiceTime < 1e-10) {
            send(first->pJob, "out");
            runningJobs.erase(first);
            first = runningJobs.begin();
        };

        if (!runningJobs.empty()) {
            scheduleNextCompletion();
        } else {
            emit(busySignal, false);
            if (hasGUI()) getDisplayString().setTagArg("i",1,"");
        }
    }
    else
    {
        if (!isIdle())
            error("job arrived while already full");

        ScheduledJob job;
        job.pJob = check_and_cast<Job *>(msg);
        if (timeout > 0 && job.pJob->getTotalQueueingTime() >= timeout) {
            // don't serve this job, just send it out
            send(job.pJob, "out");
        } else {
            job.remainingServiceTime = generateJobServiceTime(job.pJob).dbl();

            // these two are nops if there was no job running
            updateJobTimes();
            cancelEvent(endExecutionMsg);

            runningJobs.push_back(job);
            runningJobs.sort();
            scheduleNextCompletion();

            if (runningJobs.size() == 1) { // going from idle to busy
                emit(busySignal, true);
                if (hasGUI()) getDisplayString().setTagArg("i",1,"cyan");
            }
        }
    }


    if (runningJobs.size() < maxThreads) {

        // examine all input queues, and request a new job from a non empty queue
        int k = selectionStrategy->select();
        if (k >= 0)
        {
            EV << "requesting job from queue " << k << endl;
            cGate *gate = selectionStrategy->selectableGate(k);
            check_and_cast<IPassiveQueue *>(gate->getOwnerModule())->request(gate->getIndex());
        }
    }
}

void MTServer::scheduleNextCompletion() {
    // schedule next completion event
    RunningJobs::iterator first = runningJobs.begin();
    scheduleAt(simTime() + first->remainingServiceTime * runningJobs.size(), endExecutionMsg);
}

void MTServer::updateJobTimes() {
    simtime_t d = simTime() - endExecutionMsg->getSendingTime();

    // update service time and remaining time of all jobs
    for (RunningJobs::iterator it = runningJobs.begin(); it != runningJobs.end(); ++it) {
        it->remainingServiceTime -= d.dbl() / runningJobs.size();
        it->pJob->setTotalServiceTime(it->pJob->getTotalServiceTime() + d);
    }
}

void MTServer::finish() {
}

simtime_t MTServer::generateJobServiceTime(queueing::Job*) {
    simtime_t serviceTime = par("serviceTime");
    if (serviceTime <= 0.0) {
        serviceTime = 0.000001; // make it a very short job
    }
    return serviceTime;
}

bool MTServer::isIdle() {
    return runningJobs.size() < maxThreads;
}

bool MTServer::isEmpty() {
    return runningJobs.size() == 0;
}
