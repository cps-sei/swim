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

#ifndef __PLASASIM_ARRIVALMONITOR_H_
#define __PLASASIM_ARRIVALMONITOR_H_

#include <omnetpp.h>

/**
 * TODO - Generated class
 */
class ArrivalMonitor : public omnetpp::cSimpleModule
{
    omnetpp::simsignal_t interArrivalSignal;
    omnetpp::simtime_t lastArrivalTime;
    bool firstArrival;

  protected:
    virtual void initialize();
    virtual void handleMessage(omnetpp::cMessage *msg);
};

#endif
