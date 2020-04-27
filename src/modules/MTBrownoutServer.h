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

#ifndef __PLASA_MTBROWNOUTSERVER_H_
#define __PLASA_MTBROWNOUTSERVER_H_

#include "MTServer.h"
#include <map>
#include <string>

/**
 * Module class for a multi-threaded server with brownout
 */
class MTBrownoutServer : public MTServer
{
    /**
     * Map to store the request count for each server, even between
     * removals and re-additions. Note that this simulates experiments
     * in which VMs are not actually freshly booted when re-added to the
     * system. If they were, the count would have to be just per instance,
     * without persisting it.
     */
    static std::map<std::string, long> requestCount;


    /**
     * Low service requests use DB much less, so caching is not that important
     * If this is true, the are included in the caching effect simulation
     */
    bool cacheLow;

    /**
     * How many requests it takes the system to reach a somewhat stable
     * service time. It was obtained by doing this:
     * d<-loadTrace('/home/gmoreno/research/traces/wc98/wc_day53-r0-1h15m-l70.delta')
     * (sum(cumsum(d) < 900) / 3) + (sum(cumsum(d) < 2500) - sum(cumsum(d) < 900))
     * where 3 is the number of servers at the start, 900 is the warmup period, and
     * 2500 is the time at which we can visually observe in a real run that the service
     * time converges to the stable value.
     */
    int cacheRequestCount;

    /**
     * The difference in service time between the start of the run and
     * the stable state, for the requests with optional content (or high fidelity)
     */
    double cacheDelta;

    /**
     * The difference in service time between the start of the run and
     * the stable state, for the requests without optional content (or low fidelity)
     */
    double cacheDeltaLow;

    /**
     * This controls how close the exponential decay gets to stable
     * service time, and also controls the shape. This is tuned experimentally so that
     * the decay in the simulated run approximates the real run.
     */
    double cachePrecision;

    /**
     * If false, it makes clearServerCache() a noop
     */
    bool cacheClearsWhenReboot;

  protected:
    virtual simtime_t generateJobServiceTime(queueing::Job* pJob);
    virtual void initialize() override;

  public:
    void clearServerCache();
};

#endif
