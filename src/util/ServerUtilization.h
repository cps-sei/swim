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
 * ServerUtilization.h
 *
 *  Created on: Apr 2, 2013
 *      Author: gmoreno
 */

#ifndef SERVERUTILIZATION_H_
#define SERVERUTILIZATION_H_

#include <omnetpp.h>

class ServerUtilization {
    omnetpp::simtime_t start; /**<- start of the measurement period */
    omnetpp::simtime_t busyTime; /**<- the amount of time it has been busy before the last idle period */
    omnetpp::simtime_t busyPeriodStart;
    bool isBusy;

public:
    ServerUtilization();
    ServerUtilization(const ServerUtilization& su);

    /**
     * start a new measurement period
     */
    void reset();

    void busy();
    void idle();

    /**
     * get utilization since the last reset
     *
     * @return utilization as fraction (i.e., in [0,1])
     */
    double getUtilization() const;
};



#endif /* SERVERUTILIZATION_H_ */
