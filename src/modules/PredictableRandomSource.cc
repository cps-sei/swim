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

#include "PredictableRandomSource.h"
#include <math.h>
#include <string>
#include <assert.h>

using namespace std;

Define_Module(PredictableRandomSource);

const double PERIOD_LENGTH = 10; // TODO get from config
const int RNG = 1;

//#define WITH_LIMIT
#define TRENDING 1

void PredictableRandomSource::initialize() {
    lastArrivalTime = 0;
    timeInPeriod = PERIOD_LENGTH; // to force the selection of a mean
    mean = 0; // for the second method to force selection of a mean
    // for SMG comparison
    maxRate = 2.0;
    minRate = 0.0;
    direction = 0;

#if WORST_CASE
    // use these for getting close to model when LA+ utility is independent of latency
//    maxRate = 1.91;
//    minRate = 1.9;

    maxRate = 1.9;
    minRate = 0.1;

    // generate worst NLA rate sequence
    double maxTime = atof(ev.getConfig()->getConfigValue("sim-time-limit"));
    double bootDelay = getSimulation()->getSystemModule()->par("bootDelay");
    int maxPeriods = 1 + maxTime / PERIOD_LENGTH;
    int bootPeriods = bootDelay / PERIOD_LENGTH;
    int i = 0;
    rateSequence.push_back(maxRate);
    i++;
    while (i < maxPeriods) {
        if (bootPeriods == 0) {
            rateSequence.push_back(maxRate);
            i++;
        } else {
            for (int j = 0; j < bootPeriods; j++) {
                rateSequence.push_back(maxRate);
                i++;
            }
            for (int j = 0; j < bootPeriods; j++) {
                rateSequence.push_back(minRate);
                i++;
            }
        }
    }
    seqIndex = 0;
#endif
    PredictableSource::initialize();
}

#if 0
bool PredictableRandomSource::generateArrival() {
    double maxTime = atof(ev.getConfig()->getConfigValue("sim-time-limit"));
#ifdef WITH_LIMIT
    if (timeInPeriod >= PERIOD_LENGTH && lastArrivalTime < maxTime) {
#else
    if (timeInPeriod >= PERIOD_LENGTH) {
#endif
        timeInPeriod = fmod(timeInPeriod, PERIOD_LENGTH);

        // TODO this should be in initialize
        string s =  getSimulation()->getSystemModule()->par("normalServiceTime").getExpression()->str();
        double serviceTime = atof(s.substr(s.find('(') + 1).c_str());
        double serviceRate = 1.0 / serviceTime;

//        double maxRate = serviceRate * 4; // max number of servers
//        double minRate = serviceRate / 2; // one server at half-capacity

        // best for simulation
//        double maxRate = serviceRate * 3;
//        double minRate = serviceRate / 2;

        // for SMG comparison
        double maxRate = 1.9;
        double minRate = 0.1;

#ifdef WITH_LIMIT
        double maxArrivals = maxTime * 5000; //serviceRate * 2; // this two is half the number of servers
        if (arrivalTimes.size() > maxArrivals) {
            return false;
        }
#endif
        double rate = uniform(minRate, maxRate, RNG);
        mean = 1 / rate;
    }
    double interval = exponential(mean, RNG);
    timeInPeriod += interval;
    lastArrivalTime += interval;
    arrivalTimes.push_back(lastArrivalTime);
    interArrivalTimes.push_back(interval);

    return true;
}
#else
bool PredictableRandomSource::generateArrival() {
    double maxTime = atof(getEnvir()->getConfig()->getConfigValue("sim-time-limit"));

    do {
        if (mean == 0 || timeInPeriod >= PERIOD_LENGTH) {
//            if (mean == 0 && lastArrivalTime >= maxTime) {
//                return false;
//            }
            timeInPeriod = 0;

            if (true || lastArrivalTime < maxTime) { // we only change the rate within maxTime

                // TODO this should be in initialize
//                string s =  getSimulation()->getSystemModule()->par("normalServiceTime").getExpression()->str();
//                double serviceTime = atof(s.substr(s.find('(') + 1).c_str());
//                double serviceRate = 1.0 / serviceTime;

        //        double maxRate = serviceRate * 4; // max number of servers
        //        double minRate = serviceRate / 2; // one server at half-capacity

                // best for simulation
        //        double maxRate = serviceRate * 3;
        //        double minRate = serviceRate / 2;


#ifdef WITH_LIMIT
                double maxArrivals = maxTime * 5000; //serviceRate * 2; // this two is half the number of servers
                if (arrivalTimes.size() > maxArrivals) {
                    return false;
                }
#endif
#if TRENDING
                // should we change trend direction?
                if (uniform(0,1, RNG) > 0.5) {
                    if (direction == 0) {
                        // if it's close to the bounds force change away from them
                        if (rate > maxRate - 0.05) {
                            direction = -1;
                        } else if (rate < minRate + 0.05) {
                            direction = 1;
                        } else {
                            direction = (uniform(0,1, RNG) > 0.5) ? 1 : -1;
                        }
                    } else {
                        direction = 0;
                    }
                }
                if (direction != 0) {
                    if (direction < 0) {
                        //double delta = min(rate - minRate, (maxRate - minRate) * 0.50); // limite the change
                        double delta = rate - minRate; assert(delta > 0);
                        rate = uniform(rate - delta, rate, RNG);
                    } else {
                        //double delta = min(maxRate - rate, (maxRate - minRate) * 0.50); // limite the change
                        double delta = maxRate - rate; assert(delta > 0);
                        rate = uniform(rate, rate + delta, RNG);
                    }
                }
#else

#if WORST_CASE
                rate = rateSequence[seqIndex++];
#else
                rate = uniform(minRate, maxRate, RNG);
//                double u = uniform(0,1, RNG);
//                rate = 0;
//                if (u > 0.66) {
//                    rate = 1.9;
//                } else {
//                    rate = 1.1;
//                }
#endif
#endif
                if (rate > 0) {
                    mean = 1 / rate;
                    if (mean > PERIOD_LENGTH) {
                        mean = 0;
                    }
                } else {
                    mean = 0; // no events for one interval
                }
            }
        }
        if (mean > 0) {
            double interval = exponential(mean, RNG);
            timeInPeriod += interval;
            lastArrivalTime += interval;
            double interArrival = lastArrivalTime - ((arrivalTimes.empty()) ? 0 : arrivalTimes.back());
            interArrivalTimes.push_back(interArrival);
            arrivalTimes.push_back(lastArrivalTime);
            return true;
        } else {
            lastArrivalTime += PERIOD_LENGTH;
            timeInPeriod += PERIOD_LENGTH;
        }
    } while(true);
    return false;
}
#endif

void PredictableRandomSource::preload() {
    generateArrival();
}

double PredictableRandomSource::getPrediction(double startDelta,
        double windowDuration, double* pVariance, bool debug) {

    // generate arrival as needed
    double windowEnd =  simTime().dbl() + startDelta + windowDuration;
    while (windowEnd > lastArrivalTime) {
        generateArrival();
    }

    return PredictableSource::getPrediction(startDelta,
            windowDuration, pVariance, debug);
}
