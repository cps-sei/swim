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
#ifndef UTILITYSCORER_H_
#define UTILITYSCORER_H_
#include <model/Configuration.h>
#include <model/Environment.h>
#include <model/Observations.h>
#include <model/Model.h>

#define RT_THRESHOLD omnetpp::getSimulation()->getSystemModule()->par("responseTimeThreshold").doubleValue()

/*
 * MAX_SERVICE_RATE is the max number of requests/sec a single server can
 * process with normal service.
 */
#define MAX_SERVICE_RATE omnetpp::getSimulation()->getSystemModule()->par("maxServiceRate").doubleValue()

class UtilityScorer {
protected:
    UtilityScorer() {}; // no instances

    static const char* OPT_REVENUE;
    static const char* PENALTY_MULTIPLIER;

public:

    /**
     * Computes utility accrued
     */
    static double getAccruedUtility(const Model& model, const Configuration& configuration, const Environment& environment, const Observations& observations);
};

#endif /* UTILITYSCORER_H_ */
