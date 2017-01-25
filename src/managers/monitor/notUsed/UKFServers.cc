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
#include <easykf.h>
#include <UKFServers.h>
#include <iostream>

using namespace std;

using namespace ukf::UKFNAMESPACE;


/**
 * Evolution function: identity function
 */
void evolutionFunction(gsl_vector * param, gsl_vector * input, gsl_vector * output) {
    for(unsigned i = 0 ; i < input->size ; ++i) {
        gsl_vector_set(output, i, gsl_vector_get(input, i));
    }
}

double totalU; // this is sort of a hack...it should be in params

/**
 * This function applies constraints to the state
 * (or projects it onto the boundaries of the feasible space
 */
void constrain(gsl_vector * param, gsl_vector * xprev, gsl_vector * x) {
    const double deltaFactor = 0.9;
    const double LOWER_BOUND = 0.001;

    // get state
    double s1 = gsl_vector_get(x, UKFServers::KF_S_S1);
    double s2 = gsl_vector_get(x, UKFServers::KF_S_S2);

    s1 = max(LOWER_BOUND + (1 - deltaFactor) * gsl_vector_get(xprev, UKFServers::KF_S_S1), s1);
    s2 = max(LOWER_BOUND + (1 - deltaFactor) * gsl_vector_get(xprev, UKFServers::KF_S_S2), s2);

    //s1 = min(s1, s2); // s1 <= s2
    if (s1 > s2) {
        // get params
        double lambda = gsl_vector_get(param, 0);
        double delta = gsl_vector_get(param, 1);

        s1 = s2;
        s2 = max(s1, (totalU - lambda * (1 - delta) * s1) / (delta * lambda));
    }

    // update state
    gsl_vector_set(x, UKFServers::KF_S_S1, s1);
    gsl_vector_set(x, UKFServers::KF_S_S2, s2);
}

void observationFunction(gsl_vector * param, gsl_vector * x, gsl_vector * z)
{
    // get state
    double s1 = gsl_vector_get(x, UKFServers::KF_S_S1);
    double s2 = gsl_vector_get(x, UKFServers::KF_S_S2);

    // get params
    double lambda = gsl_vector_get(param, 0);
    double delta = gsl_vector_get(param, 1);

    // compute fc
    double u = min(lambda * ((1 - delta) * s1 + delta * s2), 1.0 - DBL_MIN);
    double R1 = s1 / (1 - u);
    double R2 = s2 / (1 - u);

    gsl_vector_set(z, UKFServers::KF_O_U, u);
    gsl_vector_set(z, UKFServers::KF_O_R1, R1);
    gsl_vector_set(z, UKFServers::KF_O_R2, R2);
}


UKFServers::UKFServers() : p(new ukf_param), s(new ukf_state) {
    cout << "UKFServers::UKFServers()" << endl;
}

void UKFServers::initialize(const VectorType& init) {

    // The parameters for the evolution equation
    s->params = gsl_vector_alloc(2);
    gsl_vector_set_zero(s->params); // to be set later

    // Initialization of the parameters
    p->n = 2; // GM: state vector size
    p->no = 3; // GM: observation vector size
    p->kpa = 0.0;
    p->alpha = 0.5;
    p->beta = 2.0;

    //EvolutionNoise * evolution_noise = new EvolutionAnneal(1e-1, 1.0, 1e-2);
    //EvolutionNoise * evolution_noise = new EvolutionRLS(1e-5, 0.9995);
    EvolutionNoise * evolution_noise = new EvolutionRobbinsMonro(1e-4, 1e-3);
    p->evolution_noise = evolution_noise;

    p->measurement_noise = 1e-1;
    p->prior_x= 1.0;

    // Initialization of the state and parameters
    ukf_init(*p,*s);

    // Initialize the state
    gsl_vector_set(s->xi, 0, init[0]);
    gsl_vector_set(s->xi, 1, init[1]);
}

void UKFServers::step(const VectorType& z) {

    gsl_vector * xprev = gsl_vector_alloc(p->n);
    gsl_vector_memcpy(xprev, s->xi);

    // Allocate vector
    gsl_vector * zi = gsl_vector_alloc(p->no);
    for (int i = 0; i < p->no; ++i) {
        gsl_vector_set(zi, i, z[i]);
    }

    totalU = z[0];

    // Provide the observation and iterate
    ukf_iterate(*p, *s, evolutionFunction, observationFunction, zi);

    constrain(s->params, xprev, s->xi);

    gsl_vector_free(zi);
    gsl_vector_free(xprev);
}

const VectorType UKFServers::getX() const {
    VectorType x(p->n);
    for (int i = 0; i < p->n; ++i) {
        x[i] = gsl_vector_get(s->xi, i);
    }
    return x;
}

const VectorType UKFServers::getStateVariance() const {
    VectorType v(p->n);

//    // the diagonal of the covariance matrix P has the variances for each of the state vars
//    for (int i = 0; i < 3; ++i) {
//        v[i] = gsl_matrix_get(s->Pxxi, i, i);
//    }

    // heuristic...the std dev is half the mean
    for (int i = 0; i < p->n; ++i) {
        v[i] = pow(gsl_vector_get(s->xi, i) / 2.0, 2);
    }

    return v;
}

void UKFServers::setParameters(double arrivalRate, double dimmer) {
    lambda = arrivalRate;
    delta = dimmer;
    // set known components of state
    gsl_vector_set(s->params, 0, lambda);
    gsl_vector_set(s->params, 1, delta);
}

UKFServers::~UKFServers() {
    delete(s);
    delete(p);
}

