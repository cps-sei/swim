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
/*
 * PassiveQueueDyn.h
 *
 *  Created on: Nov 14, 2012
 *      Author: gmoreno
 */

#ifndef PASSIVEQUEUEDYN_H_
#define PASSIVEQUEUEDYN_H_

#include <omnetpp.h>
#include "PassiveQueue.h"

/**
 * PassiveQueue that works with dynamically connected servers
 */
class PassiveQueueDyn : public queueing::PassiveQueue {
private:
    simsignal_t interArrivalSignal;
    simtime_t lastArrivalTime;
    bool firstArrival;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    public:
        void refresh();
};


#endif /* PASSIVEQUEUEDYN_H_ */
