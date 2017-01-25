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
 * PassiveQueueDyn.cc
 *
 *  Created on: Nov 13, 2012
 *      Author: gmoreno
 */

#include "PassiveQueueDyn.h"

Define_Module(PassiveQueueDyn);

void PassiveQueueDyn::initialize()
{
    interArrivalSignal = registerSignal("interArrival");
    firstArrival = true;
    PassiveQueue::initialize();
}

void PassiveQueueDyn::handleMessage(cMessage *msg)
{
    if (firstArrival) {
        firstArrival = false;
    } else {
        double interArrivalTime = (simTime() - lastArrivalTime).dbl();
        emit(interArrivalSignal, interArrivalTime);
    }
    lastArrivalTime = simTime();

    PassiveQueue::handleMessage(msg);
}

void PassiveQueueDyn::refresh() {
    Enter_Method_Silent("refresh()");

    /*
     * NOTE: I changed selectionStrategy from private to protected in the PassiveQueue source
     * also, I had to change the code to not complain about finding an idle server when the
     * queue is empty
     */
    if (selectionStrategy) {
        delete selectionStrategy;
    }
    selectionStrategy = queueing::SelectionStrategy::create(par("sendingAlgorithm"), this, false);
}
