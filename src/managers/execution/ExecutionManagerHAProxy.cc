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
#include "ExecutionManagerHAProxy.h"

#include <sstream>
#include <util/Utils.h>

using namespace std;
using namespace omnetpp;

Define_Module(ExecutionManagerHAProxy);

void ExecutionManagerHAProxy::initialize()
{
    ExecutionManagerModBase::initialize();
    loadBalancer.setAddress(par("HAProxySocketPath").stringValue());
    pModel->setServerThreads(getSimulation()->getSystemModule()->par("serverThreads"));
}


BootComplete* ExecutionManagerHAProxy::doAddServer(bool instantaneous) {
    int serverCount = pModel->getServers();
    if (serverCount == 0) {

        /*
         * if there should be no servers (possibly start of sim) initialize several things:
         *  -set server params in model
         *  -set brownout in haproxy
         *  -make sure all servers are disabled
         *  (all this should probably be in initialize in a second phase
         */
        double variance = 0.0;
        double mean = Utils::getMeanAndVarianceFromParameter(getParentModule()->par("serviceTime"), &variance);
        pModel->setServiceTime(mean, variance);
        mean = Utils::getMeanAndVarianceFromParameter(getParentModule()->par("lowFidelityServiceTime"), &variance);
        pModel->setLowFidelityServiceTime(mean, variance);
        double brownoutFactor = getParentModule()->par("brownoutFactor");
        pModel->setBrownoutFactor(brownoutFactor);
        doSetBrownout(brownoutFactor);
        for (int i = 1; i <= pModel->getMaxServers(); i++) {
            disableServer(i);
        }
    }


    BootComplete* pBootComplete = new BootComplete;

    // use module id as server index (starting from 1)
    pBootComplete->setModuleId(serverCount + 1);
    return pBootComplete;
}

void ExecutionManagerHAProxy::doAddServerBootComplete(
        BootComplete* bootComplete) {

    ostringstream cmd;
    cmd << "enable server servers/#";
    cmd << bootComplete->getModuleId();
    cmd << '\n';
    loadBalancer.executeCommand(cmd.str());
}

void ExecutionManagerHAProxy::disableServer(int serverNumber) {
    ostringstream cmd;
    cmd << "disable server servers/#";
    cmd << serverNumber;
    cmd << '\n';
    loadBalancer.executeCommand(cmd.str());
}

BootComplete* ExecutionManagerHAProxy::doRemoveServer() {
    int serverNumber = pModel->getServers();

    disableServer(serverNumber);

    // we don't wait, notify now
    ostringstream serverId;
    serverId << "server";
    serverId << serverNumber;
    notifyRemoveServerCompleted(serverId.str().c_str());

    // use module id as server index (starting from 1)
    BootComplete* pBootComplete = new BootComplete;
    pBootComplete->setModuleId(serverNumber);
    return pBootComplete;
}

void ExecutionManagerHAProxy::doSetBrownout(double factor) {
    ostringstream cmd;
    cmd << "set map /tmp/dimmerMap 1 ";
    cmd << 1 - factor; // translated to dimmer
    cmd << '\n';
    loadBalancer.executeCommand(cmd.str());
}
