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
#ifndef __PLASASIM_SIMPROBE_H_
#define __PLASASIM_SIMPROBE_H_

#include "IProbe.h"
#include <util/TimeWindowStats.h>

/**
 * This class collects statistics from the simulated system
 */
class SimProbe : public IProbe, omnetpp::cListener
{
public:
    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, const omnetpp::SimTime& t, cObject *details) override;
    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, bool value, cObject *details) override;
    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, double value, cObject *details) override;
    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, const char* value, cObject *details) override;

    double getBasicResponseTime();
    double getOptResponseTime();
    double getBasicThroughput();
    double getOptThroughput();
    double getUtilization(const std::string& serverName);
    double getArrivalRate();

    Observations getUpdatedObservations();
    Environment getUpdatedEnvironment();
protected:

    /* subscribed signals */
    omnetpp::simsignal_t lifeTimeSignal;
    omnetpp::simsignal_t interArrivalSignal;
    omnetpp::simsignal_t serverBusySignal;
    omnetpp::simsignal_t serverRemovedSignal;

    unsigned window; /**< time window in seconds for statistics */
    TimeWindowStats arrival;
    TimeWindowStats basicResponseTime;
    TimeWindowStats optResponseTime;

    std::map<std::string, TimeWindowStats> utilization;

    virtual int numInitStages() const {return 2;}
    virtual void initialize(int stage);
    virtual void handleMessage(omnetpp::cMessage *msg);
};

#endif
