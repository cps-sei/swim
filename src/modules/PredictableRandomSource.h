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

#ifndef PREDICTABLERANDOMSOURCE_H_
#define PREDICTABLERANDOMSOURCE_H_

#include "PredictableSource.h"

#define WORST_CASE 0 // attempt to create WC conditions

#if WORST_CASE
#include <vector>
#endif

class PredictableRandomSource: public PredictableSource {
    double timeInPeriod;
    double lastArrivalTime;
    double mean;
    double minRate;
    double maxRate;
    double rate;
    int direction; // -1 down, 0 none, 1 up
#if WORST_CASE
    std::vector<double> rateSequence;
    int seqIndex;
#endif
protected:
  virtual void preload();
  virtual bool generateArrival();
  virtual void initialize();

public:
  virtual double getPrediction(double startDelta, double windowDuration, double* pVariance, bool debug = false);

    double getMaxRate() const {
        return maxRate;
    }

    double getMinRate() const {
        return minRate;
    }
};

#endif /* PREDICTABLERANDOMSOURCE_H_ */
