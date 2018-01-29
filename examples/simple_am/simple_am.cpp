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
#include "SwimClient.h"
#include <iostream>

using namespace std;

const int NUMBER_OF_DIMMER_LEVELS = 5;
const double DIMMER_STEP = 1.0 / (NUMBER_OF_DIMMER_LEVELS - 1);
const double RT_THRESHOLD = 0.75;
const int PERIOD = 60;

void simpleAdaptationManager(SwimClient& swim) {
    while (swim.isConnected()) {
        double dimmer = swim.getDimmer();
        int servers = swim.getServers();
        int activeServers = swim.getActiveServers();
        bool isServerBooting = (servers > activeServers);
        double responseTime = swim.getAverageResponseTime();

        if (responseTime > RT_THRESHOLD) {
            if (!isServerBooting
                    && servers < swim.getMaxServers()) {
                swim.addServer();
            } else if (dimmer > 0.0) {
                dimmer = max(0.0, dimmer - DIMMER_STEP);
                swim.setDimmer(dimmer);
            }
        } else if (responseTime < RT_THRESHOLD) { // can we increase dimmer or remove servers?

            // only if there is more than one server of spare capacity
            double spareUtilization = activeServers - swim.getTotalUtilization();

            if (spareUtilization > 1) {
                if (dimmer < 1.0) {
                    dimmer = min(1.0, dimmer + DIMMER_STEP);
                    swim.setDimmer(dimmer);
                } else if (!isServerBooting && servers > 1) {
                    swim.removeServer();
                }
            }
        }

        sleep(PERIOD);
    }
}

int main(int argc, char* argv[]) {
    SwimClient swim;

    do {
        try {
            if (argc == 2) {
                swim.connect(argv[1]);
            } else if (argc == 3) {
                swim.connect(argv[1], argv[2]);
            } else {
                cout << "usage: " << argv[0] << " host [port]" << endl;
                return EXIT_FAILURE;
            }
        }
        catch(boost::system::system_error& e) {
            if (e.code() == boost::asio::error::connection_refused) {

                // wait until SWIM is accepting connections
                cout << "Waiting for SWIM..." << endl;
                sleep(3);
            } else {
                std::cout << e.what() << std::endl;
                return EXIT_FAILURE;
            }
        }
    } while(!swim.isConnected());

    cout << "Connected to SWIM" << endl;

    try {
        simpleAdaptationManager(swim);
    }
    catch(boost::system::system_error& e) {
        if (e.code() == boost::asio::error::eof) {
            cout << "SWIM ended run" << endl;
        } else {
            std::cout << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
