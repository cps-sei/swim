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
 * MMcQueue.h
 *
 *  Created on: Apr 23, 2013
 *      Author: gmoreno
 */

#ifndef MMCQUEUE_H_
#define MMCQUEUE_H_

class MMcQueue {
public:
    MMcQueue();
    static double queueingTime(double servers, double serviceRate, double arrivalMean, double arrivalStdDev);
    static double totalTime(double servers, double serviceRate, double arrivalMean, double arrivalStdDev);
    virtual ~MMcQueue();
};

#endif /* MMCQUEUE_H_ */
