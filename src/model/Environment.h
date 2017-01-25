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
#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <pladapt/Environment.h>
#include <vector>
#include <ostream>

class Environment : public pladapt::Environment {
    double arrivalMean;
    double arrivalVariance;
public:
    Environment();
    Environment(double arrivalMean, double arrivalVariance);
    virtual void printOn(std::ostream& os) const;
    double getArrivalMean() const;
    void setArrivalMean(double arrivalMean);
    double getArrivalVariance() const;
    void setArrivalVariance(double arrivalVariance);

    virtual double asDouble() const;
};

typedef std::vector<Environment> EnvironmentVector;

#endif /* ENVIRONMENT_H_ */
