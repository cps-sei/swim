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
 * GMcQueue.cc
 *
 *  Created on: Mar 13, 2013
 *      Author: gmoreno
 */

#include "GMcQueue.h"
#include <math.h>
#include <boost/math/special_functions/factorials.hpp>
#include <iostream>
#include <float.h>

using namespace std;

GMcQueue::GMcQueue() {
    // TODO Auto-generated constructor stub

}

double GMcQueue::pd(double rho, double servers)
{
    // based on formulas from http://www.mitan.co.uk/erlang/elgcmath.htm

    double factor = pow(rho, servers) /  boost::math::factorial<double>(servers);
    double sum = 0.0;
    for (double k = 0; k < servers; k++) {
        sum += pow(rho, k) / boost::math::factorial<double>(k);
    }

    return min(1.0, factor / (factor + (1.0 - rho / servers) * sum));
}

double GMcQueue::queueingTime(double servers, double serviceRate, double arrivalMean, double arrivalStdDev)
{
    double rho = 1.0 / (arrivalMean * serviceRate); // eq. 5.3.6 http://faculty.smu.edu/nbhat/chapter5.pdf
    if (rho > servers) {
        return DBL_MAX;
    }

    double cT = arrivalStdDev / arrivalMean; // coefficient of variation for arrivals
    double cS = 1.0; // coefficient of variation for service = 1 because exponential

    double p = pd(rho, servers);
//    cout << " servers=" << servers << " serviceRate=" << serviceRate << " arrivalMean=" << arrivalMean
//            << " arrivalStdDev=" << arrivalStdDev << " rho=" << rho << " pd = " << p << endl;
    return (p / (serviceRate * (servers - rho))) * ((cT * cT + cS * cS) / 2);
}


double GMcQueue::totalTime(double servers, double serviceRate, double arrivalMean, double arrivalStdDev)
{
	// TODO the interface should be changed to arrival rate so that 0 really means no arrivals (instead of infinite)
    if (arrivalMean == 0) {
        return 0; // there are no arrivals
    } else if (arrivalMean < 0) {
        return DBL_MAX;
    }

    double qt = queueingTime(servers, serviceRate, arrivalMean, arrivalStdDev);
    //cout << " qt = " << qt << endl;

    return  qt + (1 / serviceRate);
}

GMcQueue::~GMcQueue() {
    // TODO Auto-generated destructor stub
}

#ifdef TEST
const double arrivalsInPeriod[] = {5, 11, 19, 29, 30.32508491, 39};
const double TAU = 10;
int main() {
    cout << "servers\tarrivals\tarrivalMean\trt" << endl;
    for (int s = 1; s < 5; s++) {
        for (int a = 0; a < sizeof(arrivalsInPeriod) / sizeof(*arrivalsInPeriod); a++) {
            double arrivalMean = (arrivalsInPeriod[a]) ? (TAU / arrivalsInPeriod[a]) : 0;
            double rt = GMcQueue::totalTime(s, 1, arrivalMean, arrivalMean);
            cout << s << "\t" << arrivalsInPeriod[a] << "\t\t" << arrivalMean << "\t\t" << rt << endl;
        }
    }
}
#endif
