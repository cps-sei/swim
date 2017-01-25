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

#include "ArrivalMonitor.h"

using namespace omnetpp;

Define_Module(ArrivalMonitor);

void ArrivalMonitor::initialize()
{
    interArrivalSignal = registerSignal("interArrival");
    firstArrival = true;
}

void ArrivalMonitor::handleMessage(cMessage *msg)
{
    if (firstArrival) {
        firstArrival = false;
    } else {
        double interArrivalTime = (simTime() - lastArrivalTime).dbl();
        emit(interArrivalSignal, interArrivalTime);
    }
    lastArrivalTime = simTime();

    send(msg, "out");
}
