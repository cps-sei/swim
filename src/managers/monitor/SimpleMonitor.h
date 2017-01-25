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

#ifndef SIMPLEMONITOR_H_
#define SIMPLEMONITOR_H_

#include <omnetpp.h>
#include <memory>
#include "model/Model.h"
#include "IProbe.h"

#define DLL_PUBLIC __attribute__ ((visibility("default")))

/**
 * Periodically collects monitoring data from the probes and updates the model
 */
class DLL_PUBLIC SimpleMonitor : public omnetpp::cSimpleModule, omnetpp::cListener
{
  protected:
    omnetpp::cMessage *periodEvent; // pointer to the event object which we'll use for timing
    omnetpp::cMessage *periodPostEvent; // pointer to the event object which we'll use for timing (post proc)
    omnetpp::cMessage *oversamplingEvent;

    /* subscribed signals */
    omnetpp::simsignal_t serverRemovedSignal;
    omnetpp::simsignal_t serverAddedSignal;
    omnetpp::simsignal_t serverActivatedSignal;
    omnetpp::simsignal_t brownoutSetSignal;

    /* emitted signals */
    omnetpp::simsignal_t numberOfServersSignal;
    omnetpp::simsignal_t activeServersSignal;
    omnetpp::simsignal_t measuredInterarrivalAvg;
    omnetpp::simsignal_t measuredInterarrivalStdDev;
    omnetpp::simsignal_t utilitySignal;
    omnetpp::simsignal_t brownoutFactorSignal;

    Model* pModel;
    IProbe* pProbe;

    unsigned oversamplingFactor;

    virtual int numInitStages() const {return 2;}
    virtual void initialize(int stage);
    virtual void handleMessage(omnetpp::cMessage *msg);
    virtual omnetpp::simtime_t computeNextEventTime(unsigned factor) const;

    /**
     * Code to be executed periodically, according to the evaluation period,
     * and before the adaptation happens (if it is done in OMNET)
     */
    virtual void periodicHandler();

    /**
     * Code to be executed when oversampling
     *
     * Oversampling happens at a rate oversamplingFactor times faster than
     * the normal periodic handler. oversamplingFactor is a simulation
     * parameter.
     *
     * Note that every oversamplingFactor invocations, both the periodicHandler()
     * and the oversamplingHandler() will happen at the same (simulation) time.
     */
    virtual void oversamplingHandler();

    /**
     * Code to be executed periodically, according to the evaluation period,
     * and after the adaptation happens (if it is done in OMNET)
     */
    virtual void postPeriodHandler();

  public:
    SimpleMonitor();
    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, bool value, cObject *details) override;
    void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID, const char* value, cObject *details) override;

    virtual ~SimpleMonitor();
};
#endif /* SIMPLEMONITOR_H_ */
