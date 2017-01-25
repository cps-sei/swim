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

#ifndef UTIL_UTILS_H_
#define UTIL_UTILS_H_

#include <omnetpp.h>

class Utils {
public:

    /**
     * Attempts to get mean and variance from a parameter that is assigned
     * a random distribution in the ini file
     *
     * @param par parameter to get the values from
     * @param variance non-null pointer if variance is desired
     */
    static double getMeanAndVarianceFromParameter(const omnetpp::cPar& par, double* variance = nullptr);

    virtual ~Utils();
};

#endif /* UTIL_UTILS_H_ */
