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
#include <KFServers.h>
#include <math.h>
#include <float.h>

using namespace std;

KFServers::KFServers() {
    // TODO Auto-generated constructor stub

}

void KFServers::makeIdentity(KFServers::Matrix& m) const {
    for (unsigned i = 0; i < m.nrow(); i++) {
        for (unsigned j = 0; j < m.ncol(); j++) {
            m(i,j) = (i == j) ? 1.0 : 0.0;
        }
    }
}

const KFServers::Vector KFServers::getZ() const {
    return this->z;
}

void KFServers::step(KFServers::Vector& u_, const KFServers::Vector& z_) {
    // compute bounds before x is updated
    const double delta = 0.9;
    Matrix bounds(x.size(), 2); // 1st, 2nd cols are lower, upper bounds respectively
    for (unsigned i = 0; i < x.size(); i++) {
        bounds(i,0) = (1 - delta) * x(i);
        bounds(i,1) = DBL_MAX;
    }

    EKFilterType::step(u_, z_);

    for (unsigned i = 0; i < x.size(); i++) {
        x(i) = max(bounds(i,0), min(bounds(i,1), x(i)));
    }

    // prevent crossover
    if (x(0) > x(1)) {
        x(0) = (x(1) + x(0)) / 2.0;
        x(1) = x(0);
    }
}

void KFServers::makeNull(KFServers::Matrix& m) const {
    for (unsigned i = 0; i < m.nrow(); i++) {
        for (unsigned j = 0; j < m.ncol(); j++) {
            m(i,j) = 0.0;
        }
    }
}

void KFServers::makeA() {
    makeIdentity(A);
}

void KFServers::makeH() {
    double u = lambda * ((1.0 - delta) * x(0) + delta * x(1));
    double denom = pow(1 - u, 2);
    H(0,0) = lambda * (1 - delta);
    H(0,1) = lambda * delta;
    //H(1,0) = lambda * (1 - delta) * x(0) / denom + 1 / (1 - u);
    H(1,0) = (1 - lambda * delta * x(1)) / denom;
    H(1,1) = delta * lambda * x(0) / denom;
    H(2,0) = lambda * (1 - delta) * x(1) / denom;
    H(2,1) = (lambda * (delta - 1) * x(0) /*- x(0)*/ + 1) / denom;
}

void KFServers::makeV() {
    makeIdentity(V);
}

void KFServers::makeR() {
    makeNull(R);
    // from Zheng et al., a larger R encodes a suspicion that prediction errors are due to measurement error rather than parameter drift

//    double r = 0.00001;
//    R(0,0) = r;
//    R(1,1) = r / 100;
//    R(2,2) = r;

    double r = 0.000001;
    R(0,0) = r * 100;
    R(1,1) = r / 100;
    R(2,2) = r;
}

void KFServers::makeW() {
    makeIdentity(W);
}

void KFServers::makeQ() {
    /*
     *  guideline from Zheng 2008: set the diag to the square of the largest
     *  one-step change that we want to be able to track
     *  Too large makes it jittery, too small and the filter sticks and fails
     *  to respond to prediction errors
     */
    makeNull(Q);
//    Q(0,0) = pow(0.000005, 2);
//    Q(1,1) = pow(0.00001, 2);
    Q(0,0) = pow(0.0001, 2);
    Q(1,1) = pow(0.001, 2);

}

void KFServers::makeProcess() {
    // no op
}

void KFServers::makeMeasure() {
    z(0) = lambda * ((1.0 - delta) * x(0) + delta * x(1));

    double denom = 1 - z(0);
    z(1) = x(0) / denom;
    z(2) = x(1) / denom;
}


void KFServers::setParameters(double arrivalRate, double dimmer) {
    lambda = arrivalRate;
    delta = max(0.025, min(0.975, dimmer));
//    delta = dimmer;
}

void KFServers::initialize(double basicServiceTime, double optServiceTime) {
    const unsigned stateVectorLength = 2;
    const unsigned measureVectorLength = 3;
    const unsigned controlSignalLength = 0;
    const unsigned processNoiseVars = 2;
    const unsigned measurementNoiseVars = 3;
    const double estS1 = basicServiceTime;
    const double estS2 = optServiceTime;


    Vector initX(stateVectorLength);
    initX(0) = estS1;
    initX(1) = estS2;

    Matrix initP0(stateVectorLength, stateVectorLength);
    makeNull(initP0);
    for (unsigned i = 0; i < stateVectorLength; i++) {
        initP0(i,i) = pow(initX(i), 2);
    }

    setDim(stateVectorLength, controlSignalLength, processNoiseVars, measureVectorLength, measurementNoiseVars);
    init(initX, initP0);
}

KFServers::~KFServers() {
    // TODO Auto-generated destructor stub
}

