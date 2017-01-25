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

#include "PredictableSource.h"
#include "Job.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>


Define_Module(PredictableSource);

using namespace boost::accumulators;
using namespace std;

void PredictableSource::preload() {
    double arrivalTime = 0;
    const char* filePath = par("interArrivalsFile").stringValue();
    double skip = par("skip").doubleValue();

    ifstream fin(filePath);
    if (!fin) {
        error("PredictableSource %s could not read input file '%s'", this->getFullName(), filePath);
    } else {
        double timeValue;
        while (fin >> timeValue) {
            timeValue *= scale;
            arrivalTime += timeValue;
            if (arrivalTime >= skip) {
                arrivalTimes.push_back(arrivalTime - skip);
                interArrivalTimes.push_back(timeValue);
            }
        }
        fin.close();
        EV << "read " << arrivalTimes.size() << " elements from " << filePath << endl;
    }
}

bool PredictableSource::generateArrival() {
    return false;
}

void PredictableSource::initialize()
{
    SourceBase::initialize();
    scale = par("scale").doubleValue();

    nextArrivalIndex = 0;
    preload();

    // schedule the first message timer, if there is one
    if (interArrivalTimes.size() > 0) {
        scheduleAt(interArrivalTimes[nextArrivalIndex++], new cMessage("newJobTimer"));
    }
}

void PredictableSource::handleMessage(cMessage *msg)
{
    ASSERT(msg->isSelfMessage());

    if (nextArrivalIndex < interArrivalTimes.size() || generateArrival())
    {
        // reschedule the timer for the next message
        scheduleAt(simTime() + interArrivalTimes[nextArrivalIndex++], msg);

        queueing::Job *job = createJob();
        send(job, "out");
    }
    else
    {
        // finished
        delete msg;
    }

}

double PredictableSource::getPrediction(double startDelta, double windowDuration, double* pVariance, bool debug) {
    double average = 0;
    double variance = 0;

    // skip until beginning of window
    simtime_t start = simTime() + startDelta;
    unsigned index = nextArrivalIndex;
    if (index == 0) {
        index++; // the first one is not really valid because there was no previous arrival
    } else if (index > 1) {
        index--; // because nextArrivalIndex points to the next arrival to be scheduled, which means that nextArrivalIndex-1 points to the one that has been scheduled and not happened yet
    }
    while (index < arrivalTimes.size() && arrivalTimes[index] < start.dbl()) {
        index++;
    }


    if (index < arrivalTimes.size()) {
        accumulator_set<double, stats<tag::mean, tag::moment<2> > > interArrivalStats;

        double windowEnd = start.dbl() + windowDuration;
        while (index < arrivalTimes.size() && arrivalTimes[index] <= windowEnd) {
            if (debug) {
                EV << "dbginterarrival value " << interArrivalTimes[index] << " time " << arrivalTimes[index] << endl;
            }
            interArrivalStats(interArrivalTimes[index]);
            index++;
        }

        average = mean(interArrivalStats);
        variance = moment<2>(interArrivalStats);
        if (std::isnan(average)) {
            average = 0;
            variance = 0;
        }
        if (debug) {
            EV << "dbginterarrival mean " << average << endl;
        }
    }

    if (pVariance) {
        *pVariance = variance;
    }
    return average;
}

