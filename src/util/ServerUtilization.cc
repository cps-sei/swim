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
 * ServerUtilization.cc
 *
 *  Created on: Apr 2, 2013
 *      Author: gmoreno
 */

#include "ServerUtilization.h"

using namespace omnetpp;

ServerUtilization::ServerUtilization()
    : start(0), busyTime(0), busyPeriodStart(0), isBusy(false)
{
}

ServerUtilization::ServerUtilization(const ServerUtilization& su)
    : start(su.start), busyTime(su.busyTime), busyPeriodStart(su.busyPeriodStart), isBusy(su.isBusy)
{
}


/**
 * start a new measurement period
 */
void ServerUtilization::reset() {
    busyPeriodStart = start = simTime();
    busyTime = 0;
}

void ServerUtilization::busy() {
    busyPeriodStart = simTime();
    isBusy = true;
}

void ServerUtilization::idle() {
    busyTime += simTime() - busyPeriodStart;
    isBusy = false;
}


/**
 * get utilization since the last reset
 */
double ServerUtilization::getUtilization() const {
    double u = busyTime.dbl();
    if (isBusy) {
        u += (simTime() - busyPeriodStart).dbl();
    }

    double length = (simTime() - start).dbl();
    u /= length;
    if (u > 1.0) {
        EV << "utilization > 100%\n" << endl;
    }
    return u;
}

