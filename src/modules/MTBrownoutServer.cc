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

#include "MTBrownoutServer.h"
#include "Job.h"

Define_Module(MTBrownoutServer);

#define RNG 2

#define CACHING_EFFECT 1

//const double cachingCount = 15747;
//const double cachingDelta = 0.030;
//const double cachingDeltaLow = 0.012;
//const double cachingCount = 2000; //6750;

// ICAC16
//#define NO_CACHING_LOW 0 // LOW service requests use DB much less, so caching is not that important
//const double cachingCount = 6750;
//const double cachingDelta = 0.050; //0.030;

//#define NO_CACHING_LOW 1 // LOW service requests use DB much less, so caching is not that important
//const double cachingCount = 2000;
//const double cachingDelta = 0.060; //0.030;
//
//const double cachingDeltaLow = 0.0019;
//const double cachingPrecision = 0.05;

std::map<std::string, long> MTBrownoutServer::requestCount;

void MTBrownoutServer::clearServerCache() {
    if (cacheClearsWhenReboot) {
        requestCount[this->getParentModule()->getName()] = 0;
    }
}

void MTBrownoutServer::initialize() {
    MTServer::initialize();

    cacheLow = par("cacheLow");
    cacheRequestCount = par("cacheRequestCount");
    cacheDelta = par("cacheDelta");
    cacheDeltaLow = par("cacheDeltaLow");
    cachePrecision = par("cachePrecision");
    cacheClearsWhenReboot = par("cacheClearsWhenReboot");
}

simtime_t MTBrownoutServer::generateJobServiceTime(queueing::Job* pJob)  {
    double brownoutFactor = par("brownoutFactor");
    double u = uniform(0, 1, RNG);
    simtime_t st = 0;
    if (u > brownoutFactor) {
        st = MTServer::generateJobServiceTime(pJob);
    } else {
        pJob->setKind(1); // mark the job as low fidelity
        simtime_t serviceTime = par("lowFidelityServiceTime");
        if (serviceTime <= 0.0) {
            serviceTime = 0.000001; // make it a very short job
        }
        st = serviceTime;
    }

#if CACHING_EFFECT
    // exponential decay (simulate caching)

    if (cacheLow || pJob->getKind() != 1) {
        double delta = (pJob->getKind() == 1) ? cacheDeltaLow : cacheDelta;
        double lambda = (-1.0 / cacheRequestCount) * (log(cachePrecision * delta) - log(delta));

        /* note that this assumes that this module is inside an AppServer module with a unique name */
        long myRequestCount = requestCount[this->getParentModule()->getName()];
        st += delta * exp(-lambda * myRequestCount);
        requestCount[this->getParentModule()->getName()] = ++myRequestCount;
    }
#endif

    emit(registerSignal("serviceTime"), (pJob->getKind() == 1) ? -st.dbl() : st.dbl());

    return st;
}
