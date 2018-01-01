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
 * PredictableRateSource.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: gmoreno
 */

#include "PredictableRateSource.h"
#include <fstream>
#include <math.h>

Define_Module(PredictableRateSource);

using namespace std;

void PredictableRateSource::preload() {
    double arrivalTime = 0;
    const char* filePath = par("rateFile").stringValue();
    ifstream fin(filePath);
    if (!fin) {
        error("PredictableSource %s could not read input file '%s'", this->getFullName(), filePath);
    } else {
        double timeValue;
        string line;
        while (getline(fin, line, ',')) {
            if (line.empty() || line[0] == '\n') {
                continue;
            }
            double rate = atof(line.c_str());

            double duration;
            getline(fin, line);
            duration = atof(line.c_str());
            duration *= scale;
            timeValue = (1 / rate) * scale;
            while (duration > 0) {
                duration -= timeValue;
                arrivalTime += timeValue;
                arrivalTimes.push_back(arrivalTime);
                interArrivalTimes.push_back(timeValue);
            }
        }
        fin.close();
    }
}
