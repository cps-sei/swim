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
#ifndef KFSERVERS_H_
#define KFSERVERS_H_
#include <kalman/ekfilter.hpp>

typedef Kalman::EKFilter<double,0> EKFilterType;

class KFServers : public EKFilterType {
    // should this be measurement variables?
    double lambda; // arrival rate
    double delta; // dimmer
public:
    KFServers();
    void initialize(double basicServiceTime, double optServiceTime);
    void setParameters(double arrivalRate, double dimmer);
    const Vector getZ() const;

    /**
     * Overriden to bound the state estimate as in Zheng et al. sect 2.3
     * TODO the base class should declare this virtual
     */
    void step(Vector& u_, const Vector& z_);

    virtual ~KFServers();

protected:
    void makeA();
    void makeH();
    void makeV();
    void makeR();
    void makeW();
    void makeQ();
    void makeProcess();
    void makeMeasure();

    void makeIdentity(Matrix& m) const;
    void makeNull(Matrix& m) const;
};

#endif /* KFSERVERS_H_ */
