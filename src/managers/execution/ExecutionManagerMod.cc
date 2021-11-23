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
#include "ExecutionManagerMod.h"
#include <sstream>
#include <boost/tokenizer.hpp>
#include <cstdlib>
#include "PassiveQueue.h"
#include "modules/MTServer.h"
#include "modules/MTBrownoutServer.h"
#include <util/Utils.h>


using namespace std;
using namespace omnetpp;

Define_Module(ExecutionManagerMod);

const char* SERVER_MODULE_NAME = "server";
const char* LOAD_BALANCER_MODULE_NAME = "loadBalancer";
const char* INTERNAL_SERVER_MODULE_NAME = "server";
const char* INTERNAL_QUEUE_MODULE_NAME = "queue";
const char* SINK_MODULE_NAME = "classifier";

ExecutionManagerMod::ExecutionManagerMod() {
    serverBusySignalId = registerSignal("busy");
}


void ExecutionManagerMod::initialize() {
    ExecutionManagerModBase::initialize();
    getSimulation()->getSystemModule()->subscribe(serverBusySignalId, this);
}

void ExecutionManagerMod::handleMessage(cMessage *msg) {
    RemoveComplete* removeCompleteMsg = dynamic_cast<RemoveComplete *>(msg);
    if (removeCompleteMsg) {
        cModule* module = getSimulation()->getModule(removeCompleteMsg->getModuleId());
        module->gate("out")->disconnect();
        module->deleteModule();
        serversBeingRemoved.erase(removeCompleteMsg->getModuleId());

        cancelAndDelete(removeCompleteMsg);
    } else {
        ExecutionManagerModBase::handleMessage(msg);
    }
}


void ExecutionManagerMod::doAddServerBootComplete(BootComplete* bootComplete) {
    cModule *server = getSimulation()->getModule(bootComplete->getModuleId());
    cModule* loadBalancer = getParentModule()->getSubmodule(
            LOAD_BALANCER_MODULE_NAME);
    cModule* sink = getParentModule()->getSubmodule(SINK_MODULE_NAME);
    // connect gates
    loadBalancer->getOrCreateFirstUnconnectedGate("out", 0, false, true)->connectTo(
            server->gate("in"));
    server->gate("out")->connectTo(
            sink->getOrCreateFirstUnconnectedGate("in", 0, false, true));
}


ExecutionManagerMod::~ExecutionManagerMod() {
}


BootComplete* ExecutionManagerMod::doAddServer(bool instantaneous) {
    // find factory object
    cModuleType *moduleType = cModuleType::get("plasa.modules.AppServer");

    int serverCount = pModel->getServers();
    stringstream name;
    name << SERVER_MODULE_NAME;
    name << serverCount + 1;
    cModule *module = moduleType->create(name.str().c_str(), getParentModule());

    // setup parameters
    module->finalizeParameters();
    module->buildInside();

    // copy all params of the server inside the appserver module from the template
    cModule* pNewSubmodule = module->getSubmodule(INTERNAL_SERVER_MODULE_NAME);
    if (serverCount >= 1) {
        // copy from an existing server
        stringstream templateName;
        templateName << SERVER_MODULE_NAME;
        templateName << 1;
        cModule* pTemplateSubmodule = getParentModule()->getSubmodule(templateName.str().c_str())->getSubmodule(INTERNAL_SERVER_MODULE_NAME);

        for (int i = 0; i < pTemplateSubmodule->getNumParams(); i++) {
            pNewSubmodule->par(i) = pTemplateSubmodule->par(i);
        }
    } else {
        // if it's the first server, we need to set the parameters common to all servers in the model
        pModel->setServerThreads(pNewSubmodule->par("threads"));
        double variance = 0.0;
        double mean = Utils::getMeanAndVarianceFromParameter(pNewSubmodule->par("serviceTime"), &variance);
        pModel->setServiceTime(mean, variance);
        mean = Utils::getMeanAndVarianceFromParameter(pNewSubmodule->par("lowFidelityServiceTime"), &variance);
        pModel->setLowFidelityServiceTime(mean, variance);
        pModel->setBrownoutFactor(pNewSubmodule->par("brownoutFactor"));
    }

    // create activation message
    module->scheduleStart(simTime());
    module->callInitialize();

    BootComplete* bootComplete = new BootComplete;
    bootComplete->setModuleId(module->getId());
    return bootComplete;
}

BootComplete*  ExecutionManagerMod::doRemoveServer() {
    int serverCount = pModel->getServers();
    stringstream name;
    name << SERVER_MODULE_NAME;
    name << serverCount;
    cModule *module = getParentModule()->getSubmodule(name.str().c_str());
    ASSERT(module != nullptr);

    // disconnect module from load balancer
    cGate* pInGate = module->gate("in");
    if (pInGate->isConnected()) {
        cGate *otherEnd = pInGate->getPathStartGate();
        otherEnd->disconnect();
        ASSERT(otherEnd->getIndex() == otherEnd->getVectorSize() - 1);

        // reduce the size of the out gate in the queue module
        otherEnd->getOwnerModule()->setGateSize(otherEnd->getName(), otherEnd->getVectorSize() - 1);
        //TODO this is probably leaking memory because the gate may not be being deleted
    }

    // this will signal iProbe so that the entry that monitors this server is removed
    notifyRemoveServerCompleted(module->getName());

    // give module to be removed a new unique name
    stringstream newName;
    newName << "R-";
    newName << SERVER_MODULE_NAME;
    newName << module->getId();
    module->setName(newName.str().c_str());
    serversBeingRemoved.insert(module->getId());

    // check to see if we can delete the server immediately (or if it's busy)
    if (isServerBeingRemoveEmpty(module->getId())) {
        completeServerRemoval(module->getId());
    }

    BootComplete* bootComplete = new BootComplete;
    bootComplete->setModuleId(module->getId());
    return bootComplete;
}

void ExecutionManagerMod::doSetBrownout(double factor) {
    int serverCount = pModel->getServers();
    for (int s = 1; s <= serverCount; s++) {
        stringstream name;
        name << SERVER_MODULE_NAME;
        name << s;

        cModule* module = getParentModule()->getSubmodule(name.str().c_str());
        module->getSubmodule("server")->par("brownoutFactor").setDoubleValue(factor);
    }

    // set brownout on servers being removed
    for (int moduleId : serversBeingRemoved) {
        cModule* module = getSimulation()->getModule(moduleId);
        module->getSubmodule("server")->par("brownoutFactor").setDoubleValue(factor);
    }
}


void ExecutionManagerMod::completeServerRemoval(int serverBeingRemovedModuleId) {
    Enter_Method("sendMe()");

    // clear cache for server, so that the next time it is instantiated it is fresh
    cModule* module = getSimulation()->getModule(serverBeingRemovedModuleId);
    check_and_cast<MTBrownoutServer*>(module->getSubmodule(INTERNAL_SERVER_MODULE_NAME))->clearServerCache();

    RemoveComplete *completeRemoveMsg = new RemoveComplete;
    completeRemoveMsg->setModuleId(serverBeingRemovedModuleId);
    cout << "scheduled complete remove at " << simTime() << endl;
    scheduleAt(simTime(), completeRemoveMsg);
}

bool ExecutionManagerMod::isServerBeingRemoveEmpty(int serverBeingRemovedModuleId) {
    bool isEmpty = false;
    cModule* module = getSimulation()->getModule(serverBeingRemovedModuleId);
    MTServer* internalServer = check_and_cast<MTServer*> (module->getSubmodule(INTERNAL_SERVER_MODULE_NAME));
    if (internalServer->isEmpty()) {
        queueing::PassiveQueue* queue = check_and_cast<queueing::PassiveQueue*> (module->getSubmodule(INTERNAL_QUEUE_MODULE_NAME));
        if (queue->length() == 0) {
            isEmpty = true;
        }
    }
    return isEmpty;
}

void ExecutionManagerMod::receiveSignal(cComponent *source, simsignal_t signalID, bool value, cObject *details) {
    if (signalID == serverBusySignalId && value == false) {
        int serverModuleId = source->getParentModule()->getId();
        if (serversBeingRemoved.find(serverModuleId) != serversBeingRemoved.end()) {
            if (isServerBeingRemoveEmpty(serverModuleId)) {
                completeServerRemoval(serverModuleId);
            }
        }
    }
}
