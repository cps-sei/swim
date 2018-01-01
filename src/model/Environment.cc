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
#include "Environment.h"

Environment::Environment() : arrivalMean(0), arrivalVariance(0) {}

Environment::Environment(double arrivalMean, double arrivalStdDev)
    : arrivalMean(arrivalMean), arrivalVariance(arrivalStdDev) {};

double Environment::getArrivalMean() const {
    return arrivalMean;
}

void Environment::setArrivalMean(double arrivalMean) {
    this->arrivalMean = arrivalMean;
}

double Environment::getArrivalVariance() const {
    return arrivalVariance;
}

void Environment::setArrivalVariance(double arrivalVariance) {
    this->arrivalVariance = arrivalVariance;
}

void Environment::printOn(std::ostream& os) const {
    os << "environment[interArrival mean=" << arrivalMean << ", variance=" << arrivalVariance << "]";
}

std::ostream& operator<<(std::ostream& os, const Environment& env) {
    env.printOn(os);
    return os;
}

double Environment::asDouble() const {
    return arrivalMean;
}

