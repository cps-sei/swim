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
#include <cassert>
#include <unistd.h>

using namespace std;

int main() {

    SwimClient swim;

    swim.connect("localhost");

    int max = swim.getMaxServers();
    cout << "Max servers=" << max << endl;
    int servers = swim.getServers();
    cout << "Servers=" << servers << endl;

    for (int s = 1; s <= servers; s++) {
        cout << "Utilization server " << s << "=" << swim.getUtilization(s) << endl;
    }

    if (servers == max) {
        swim.removeServer();
        servers--;
    }

    swim.addServer();
    sleep(1);
    int newServers = swim.getServers();
    assert(newServers == servers + 1);
    int active = swim.getActiveServers();
    assert(active == servers); // requires boodDelay > 1
    cout << "waiting for server to boot ";
    while (active == servers) {
        cout << '.';
        cout.flush();
        sleep(5);
        active = swim.getActiveServers();
    }
    assert(active = newServers);
    cout << "done" << endl;

    swim.removeServer();
    sleep(1);
    assert(swim.getServers() == newServers - 1);

    swim.setDimmer(0.12);
    sleep(1);
    assert(swim.getDimmer() == 0.12);

    cout << "basic rt=" << swim.getBasicResponseTime() << endl;
    cout << "opt rt=" <<  swim.getOptionalResponseTime() << endl;
    cout << "basic tput=" << swim.getBasicThroughput() << endl;
    cout << "opt tput=" <<  swim.getOptionalThroughput() << endl;
    cout << "arrival rate=" << swim.getArrivalRate() << endl;

    return EXIT_SUCCESS;
}
