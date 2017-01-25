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
#include "UtilityScorer.h"
#include <algorithm>
#include <cfloat>
#include <omnetpp.h>


const char* UtilityScorer::OPT_REVENUE = "optRevenue";
const char* UtilityScorer::PENALTY_MULTIPLIER = "penaltyMultiplier";

/**
 * returns utility per unit of time;
 */
double UtilityScorer::getAccruedUtility(const Model& model, const Configuration& configuration, const Environment& environment, const Observations& observations)
{
    double brownoutRevenue = 1;
    double normalRevenue = 1.5;
    double maxThroughput = model.getMaxServers() * MAX_SERVICE_RATE;

    const auto& sysmodule = omnetpp::getSimulation()->getSystemModule();
    if (sysmodule->hasPar(OPT_REVENUE)) {
        normalRevenue = sysmodule->par(OPT_REVENUE);
    }

    double latePenalty = maxThroughput * normalRevenue;

    if (sysmodule->hasPar(PENALTY_MULTIPLIER)) {
        latePenalty *= sysmodule->par(PENALTY_MULTIPLIER).doubleValue();
    }

    // TODO this is arrival rate, not really throughput
    // Using arrival rate is just an approximation. Fixing it would require computing the throughput with a perf model
    double throughput = (environment.getArrivalMean() > 0) ? (1 / environment.getArrivalMean()) : 0.0;

    double brownoutFactor = model.brownoutLevelToFactor(configuration.getBrownOutLevel());

    double positiveUtility = round((throughput * (brownoutFactor * brownoutRevenue + (1 - brownoutFactor) * normalRevenue)));

    double utility = ((observations.avgResponseTime>RT_THRESHOLD || observations.avgResponseTime < 0) ? std::min(0.0, throughput * normalRevenue - latePenalty) : positiveUtility);

    return utility / model.getEvaluationPeriod();
}

