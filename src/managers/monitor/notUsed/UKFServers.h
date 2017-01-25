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
#ifndef UKFSERVERS_H_
#define UKFSERVERS_H_
#include <vector>

#define UKFNAMESPACE state

#ifndef UKF_H
// forward decl.
namespace ukf {
    namespace UKFNAMESPACE {
        class ukf_param;
        class ukf_state;
    }
}
#endif

typedef std::vector<double> VectorType;

class UKFServers {
    // Definition of the parameters and state variables
    ukf::UKFNAMESPACE::ukf_param* p;
    ukf::UKFNAMESPACE::ukf_state* s;

    double lambda;
    double delta;

public:
    UKFServers();
    void initialize(const VectorType& init);
    void setParameters(double arrivalRate, double dimmer);

    void step(const VectorType& z);
    const VectorType getX() const;
    const VectorType getStateVariance() const;

    virtual ~UKFServers();

    enum StateComponents { KF_S_S1, KF_S_S2 };
    enum ObservationComponents { KF_O_U, KF_O_R1, KF_O_R2};
};

#endif /* UKFSERVERS_H_ */
