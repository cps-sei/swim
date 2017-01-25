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
 * MMcQueue.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: gmoreno
 */

#include "MMcQueue.h"
#include <math.h>
#include <boost/math/special_functions/factorials.hpp>
#include <iostream>
#include <float.h>

MMcQueue::MMcQueue() {
    // TODO Auto-generated constructor stub

}

double MMcQueue::queueingTime(double servers, double serviceRate, double arrivalMean, double arrivalStdDev)
{
    double l = 1 / arrivalMean; // lambda
    double a = l / serviceRate;
    double rho = l / (servers * serviceRate); // pg 400 Chiulli
    if (rho > 1) {
        return DBL_MAX;
    }


    // was pg 401 Chiulli, but changed it, so new citation is needed
    double factor = pow(a, servers) /  (boost::math::factorial<double>(servers) * (1 - rho));

    double sum = 0.0;
    for (double k = 0; k < servers; k++) {
        sum += pow(a, k) / boost::math::factorial<double>(k);
    }

    double p0 = 1 / (sum + factor);
    double lq = p0 * (pow(a, servers) / boost::math::factorial<double>(servers))
            * (rho / pow(1 - rho, 2));

    double wq = lq / l;
    return wq;
}


double MMcQueue::totalTime(double servers, double serviceRate, double arrivalMean, double arrivalStdDev)
{
    if (arrivalMean == 0) {
        return 0; // there are no arrivals
    } else if (arrivalMean < 0) {
        return DBL_MAX;
    }

    double qt = queueingTime(servers, serviceRate, arrivalMean, arrivalStdDev);
    //cout << " qt = " << qt << endl;

    return  qt + (1 / serviceRate);
}


MMcQueue::~MMcQueue() {
    // TODO Auto-generated destructor stub
}

#ifdef TEST
using namespace std;
const double arrivalsInPeriod[] = {0, 11, 19};
const double TAU = 10000;
int main() {
    cout << "servers\tarrivals\tarrivalMean\trt" << endl;
    for (int s = 1; s < 5; s++) {
        for (int a = 0; a < 3; a++) {
            double arrivalMean = (arrivalsInPeriod[a]) ? (TAU / arrivalsInPeriod[a]) : 0;
            double rt = MMcQueue::totalTime(s, 0.001, arrivalMean, arrivalMean);
            cout << s << "\t" << arrivalsInPeriod[a] << "\t\t" << arrivalMean << "\t\t" << rt << endl;
        }
    }
}
#endif

#ifdef TEST2
using namespace std;
const double arrivalMeans[] = {0.388705, 2.38613, 3.32234};
const double TAU = 10000;
int main() {
    cout << "servers\tarrivals\tarrivalMean\trt" << endl;
    for (int s = 1; s < 5; s++) {
        for (int a = 0; a < 3; a++) {
	  double arrivalMean = arrivalMeans[a];
	  double rt = MMcQueue::totalTime(s, 1, arrivalMean, arrivalMean);
	  cout << s << "\t" << arrivalMeans[a] << "\t\t" << arrivalMean << "\t\t" << rt << endl;
        }
    }
}
#endif
