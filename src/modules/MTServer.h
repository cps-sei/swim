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

#ifndef MTSERVER_H_
#define MTSERVER_H_

#include <IServer.h>
#include <list>

namespace queueing {
    class Job;
    class SelectionStrategy;
}

class MTServer: public omnetpp::cSimpleModule, public queueing::IServer {
protected:
    struct ScheduledJob {
        queueing::Job* pJob;
        double remainingServiceTime; // as if it was not sharing the processor
        bool operator<(const ScheduledJob& b) const {
            return remainingServiceTime < b.remainingServiceTime;
        }
    };

    cMessage *endExecutionMsg;
    queueing::SelectionStrategy* selectionStrategy;
    unsigned maxThreads;
    simsignal_t busySignal;

    typedef std::list<ScheduledJob> RunningJobs;
    RunningJobs runningJobs;
    simtime_t timeout;

    virtual void updateJobTimes();
    virtual void scheduleNextCompletion();

    virtual simtime_t generateJobServiceTime(queueing::Job* pJob);

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
public:
    MTServer();
    virtual ~MTServer();

    /**
     * This actually returns true if it can take one more job, even if it's not idle
     * This is this way to follow the semantics that PassiveQueue expect for this
     */
    virtual bool isIdle();

    virtual bool isEmpty();
};

#endif /* MTSERVER_H_ */
