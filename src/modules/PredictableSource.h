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

#ifndef __SELFADAPTIVE_PREDICTABLESOURCE_H_
#define __SELFADAPTIVE_PREDICTABLESOURCE_H_

#include "Source.h"
#include <vector>

/**
 * Generates job with predictable interarrival time
 */
class PredictableSource : public queueing::SourceBase
{
protected:
    std::vector<double> arrivalTimes;
    std::vector<double> interArrivalTimes;
    unsigned nextArrivalIndex;
    double scale;

  protected:
    /**
     * Preload arrival times
     * Must generate at least one arrival
     */
    virtual void preload();

    /**
     * Called when more arrivals are needed
     *
     * @return true if it generated at least one more arrival
     */
    virtual bool generateArrival();

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  public:
    virtual double getPrediction(double startDelta, double windowDuration, double* pVariance, bool debug = false);

};

#endif
