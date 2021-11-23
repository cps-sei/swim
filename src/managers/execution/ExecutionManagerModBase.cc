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
#include "ExecutionManagerModBase.h"
#include <sstream>
#include <cstdlib>


using namespace std;
using namespace omnetpp;

const char* ExecutionManagerModBase::SIG_SERVER_REMOVED = "serverRemoved";
const char* ExecutionManagerModBase::SIG_SERVER_ADDED = "serverAdded";
const char* ExecutionManagerModBase::SIG_SERVER_ACTIVATED = "serverActivated";
const char* ExecutionManagerModBase::SIG_BROWNOUT_SET = "brownoutSet";


ExecutionManagerModBase::ExecutionManagerModBase() : testMsg(0) {
}

void ExecutionManagerModBase::initialize() {
    pModel = check_and_cast<Model*> (getParentModule()->getSubmodule("model"));
    serverRemovedSignal = registerSignal(SIG_SERVER_REMOVED);
    serverAddedSignal = registerSignal(SIG_SERVER_ADDED);
    serverActivatedSignal = registerSignal(SIG_SERVER_ACTIVATED);
    brownoutSetSignal = registerSignal(SIG_BROWNOUT_SET);
//    testMsg = new cMessage;
//    testMsg->setKind(0);
//    scheduleAt(simTime() + 1, testMsg);
}

void ExecutionManagerModBase::handleMessage(cMessage* msg) {
    if (msg == testMsg) {
        if (msg->getKind() == 0) {
            addServer();
            msg->setKind(1);
            scheduleAt(simTime() + 31, msg);
        } else {
            removeServer();
        }
        return;
    }
    BootComplete* bootComplete = check_and_cast<BootComplete *>(msg);

    doAddServerBootComplete(bootComplete);

    //  notify add complete to model
    pModel->serverBecameActive();
    emit(serverActivatedSignal, true);

    cout << "t=" << simTime() << " addServer() complete" << endl;

    // remove from pending
    BootCompletes::iterator it = pendingMessages.find(bootComplete);
    ASSERT(it != pendingMessages.end());
    delete *it;
    pendingMessages.erase(it);
}

ExecutionManagerModBase::~ExecutionManagerModBase() {
    for (BootCompletes::iterator it = pendingMessages.begin(); it != pendingMessages.end(); ++it) {
        cancelAndDelete(*it);
    }
    cancelAndDelete(testMsg);
}

void ExecutionManagerModBase::addServer() {
    cout << "t=" << simTime() << " executing addServer()" << endl;
    addServerLatencyOptional();
}

void ExecutionManagerModBase::addServerLatencyOptional(bool instantaneous) {
    Enter_Method("addServer()");
    int serverCount = pModel->getServers();
    ASSERT(serverCount < pModel->getMaxServers());

    BootComplete* bootComplete = doAddServer(instantaneous);

    double bootDelay = 0;
    if (!instantaneous) {
        bootDelay = omnetpp::getSimulation()->getSystemModule()->par("bootDelay");
        cout << "adding server with latency=" << bootDelay << endl;
    }

    pModel->addServer(bootDelay);
    emit(serverAddedSignal, true);

    pendingMessages.insert(bootComplete);
    if (bootDelay == 0) {
        handleMessage(bootComplete);
    } else {
        scheduleAt(simTime() + bootDelay, bootComplete);
    }
}

void ExecutionManagerModBase::removeServer() {
    Enter_Method("removeServer()");
    cout << "t=" << simTime() << " executing removeServer()" << endl;
    int serverCount = pModel->getServers();
    ASSERT(serverCount > 1);

    BootComplete* pBootComplete = doRemoveServer();

    pModel->removeServer();

    // cancel boot complete event if server being removed is booting
    for (BootCompletes::iterator it = pendingMessages.begin(); it != pendingMessages.end(); ++it) {
        if ((*it)->getModuleId() == pBootComplete->getModuleId()) {
            cancelAndDelete(*it);
            pendingMessages.erase(it);
            break;
        }
    }
    delete pBootComplete;
    cout << "t=" << simTime() << " done executing removeServer()" << endl;
}

void ExecutionManagerModBase::setBrownout(double factor) {
    Enter_Method("setBrownout()");
    cout << "t=" << simTime() << " executing setDimmer(" << 1.0 - factor << ")" << endl;
    pModel->setBrownoutFactor(factor);
    doSetBrownout(factor);
    emit(brownoutSetSignal, true);
}

void ExecutionManagerModBase::notifyRemoveServerCompleted(const char* serverId) {

    // emit signal to notify others (notably iProbe)
    emit(serverRemovedSignal, serverId);
}
