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
#ifndef HAPROXYPROBE_H_
#define HAPROXYPROBE_H_

#include <boost/asio.hpp>
#include "IProbe.h"
#include "util/HAProxySocketCommand.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <map>
#include <string>
#include "model/Model.h"


/**
 * This class collects statistics from the system running with HAProxy
 */
class HAProxyProbe : public IProbe, omnetpp::cListener
{
    HAProxySocketCommand loadBalancer;
    double getAverageRequestRate();

    boost::asio::ip::tcp::iostream tcpStream;
    omnetpp::cMessage *initEvent; /**< event to trigger trace replay */
    omnetpp::cMessage *endWarmupEvent; /**< event to get time from HAP host */

  protected:
    long long lastRequestCounter;
    boost::posix_time::ptime lastCounterResetTime;

    boost::posix_time::ptime lastEnvironmentUpdate;
    boost::posix_time::ptime lastObservationsUpdate;
    Environment environment;
    Observations observations;
    std::map<std::string, double> utilization;

    Model* pModel;

    omnetpp::simsignal_t serverRemovedSignal;

    virtual void initialize(int stage);
    virtual void handleMessage(omnetpp::cMessage *msg);

    double getBasicResponseTime();
    double getOptResponseTime();
    double getBasicThroughput();
    double getOptThroughput();
    double getUtilization(const std::string& serverName);
    double getArrivalRate();

    Observations getUpdatedObservations();
    Environment getUpdatedEnvironment();

    void connectToLogFileProbe();
    void sendReplayTraceSyncSignal();

  public:
    const unsigned MEASUREMENT_OBSOLECENSE_MSEC = 500;

    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, const char* value, cObject *details) override;

    virtual ~HAProxyProbe();
};

#endif /* HAPROXYPROBE_H_ */
