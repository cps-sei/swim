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
#include "Model.h"
#include <omnetpp.h>
#include <managers/execution/ExecutionManagerModBase.h>
#include "modules/PredictableRandomSource.h"
#include <sstream>
#include <math.h>
#include <iostream>
#include <assert.h>
#include <util/Utils.h>

using namespace std;

#define LOCDEBUG 0

Define_Module(Model);

const char* Model::HORIZON_PAR = "horizon";

Model::Model()
    : activeServerCountLast(0), timeActiveServerCountLast(0.0), activeServers(0)
{
}

Model::~Model() {
}

void Model::addExpectedChange(double time, ModelChange change)
{
    ModelChangeEvent event;
    event.startTime = simTime().dbl();
    event.time = time;
    event.change = change;
    events.insert(event);
}

void Model::removeExpectedChange()
{
    if (!events.empty()) {
        events.erase(--events.end());
    } else {
        std::cout << "removeExpectedChange(): serverCount "
                << getServers() << " activeServerCount "
                << activeServers << std::endl;
    }
}

int Model::getActiveServerCountIn(double deltaTime)
{
    /*
     * We don't keep a past history, but if we need to know what was the active
     * server count an infinitesimal instant before now, we can use a negative delta time
     */
    if (deltaTime < 0) {
        return (timeActiveServerCountLast < simTime().dbl()) ? activeServers : activeServerCountLast;
    }

    int servers = activeServers;

    double currentTime = simTime().dbl();
    ModelChangeEvents::iterator it = events.begin();
    while (it != events.end() && it->time <= currentTime + deltaTime) {
        if (it->change == SERVER_ONLINE) {
            servers++;
        }
        it++;
    }

    return servers;
}

void Model::addServer(double bootDelay)
{
    ASSERT(!isServerBooting()); // only one add server tactic at a time
    addExpectedChange(simTime().dbl() + bootDelay, Model::SERVER_ONLINE);

#if LOCDEBUG
    std::cout << simTime().dbl() << ": " << "addServer(" << bootDelay << "): serverCount=" << getServers() << " active=" << activeServers << " expected=" << events.size() << std::endl;
#endif
}


void Model::serverBecameActive()
{
    activeServerCountLast = activeServers;
    timeActiveServerCountLast = simTime().dbl();

    /* remove expected change...assume it is the first SERVER_ONLINE */
    ModelChangeEvents::iterator it = events.begin();
    while (it != events.end() && it->change != Model::SERVER_ONLINE) {
        it++;
    }
    assert(it != events.end()); // there must be an expected change for this
    events.erase(it);

    activeServers++;

#if LOCDEBUG
    std::cout << simTime().dbl() << ": " << "serverBecameActive(): serverCount=" << getServers() << " active=" << activeServers << " expected=" << events.size() << std::endl;
    if (events.size() > 0) {
        cout << simTime().dbl() << "expected event time=" << events.begin()->time
                << string(((events.begin()->time > simTime().dbl()) ? " > " : " <= "))
                        << " current time" << endl;
    }
#endif

}

void Model::removeServer()
{
    if (isServerBooting()) {

        /* the server we're removing is not active yet */
        removeExpectedChange();
    } else {
        activeServerCountLast = activeServers;
        timeActiveServerCountLast = simTime().dbl();

        activeServers--;
    }

#if LOCDEBUG
    std::cout << simTime().dbl() << ": " << "removeServer(): serverCount=" << getServers() << " active=" << activeServers << " expected=" << events.size() << std::endl;
#endif

}

void Model::setBrownoutFactor(double factor) {
    brownoutFactor = factor;
}

double Model::getBrownoutFactor() const {
    return brownoutFactor;
}

void Model::setDimmerFactor(double factor) {
    setBrownoutFactor(1.0 - factor);
}

double Model::getDimmerFactor() const {
    return 1.0 - getBrownoutFactor();
}

int const Model::getActiveServers() const {
    return activeServers;
}

int const Model::getServers() const {
    return (isServerBooting() ? activeServers + 1 : activeServers);
}

void Model::initialize(int stage) {
    if (stage == 0) {
        // get parameters
        maxServers = getSimulation()->getSystemModule()->par("maxServers");
        evaluationPeriod = getSimulation()->getSystemModule()->par("evaluationPeriod").doubleValue();
        bootDelay = Utils::getMeanAndVarianceFromParameter(
                                getSimulation()->getSystemModule()->par("bootDelay"));

        horizon = -1;
        if (hasPar(HORIZON_PAR)) {
            horizon = par("horizon");
        }
        if (horizon < 0) {
            horizon = max(5.0, ceil(bootDelay / evaluationPeriod) * (maxServers - 1) + 1);
        }

        numberOfBrownoutLevels = getSimulation()->getSystemModule()->par("numberOfBrownoutLevels");
        dimmerMargin = getSimulation()->getSystemModule()->par("dimmerMargin");
        lowerDimmerMargin = par("lowerDimmerMargin");
    } else {
        // start servers
        ExecutionManagerModBase* pExecMgr = check_and_cast<ExecutionManagerModBase*> (getParentModule()->getSubmodule("executionManager"));
        int initialServers = getSimulation()->getSystemModule()->par("initialServers");
        while (initialServers > 0) {
            pExecMgr->addServerLatencyOptional(true);
            initialServers--;
        }
    }
}

bool Model::isServerBooting() const {
    bool isBooting = false;

    if (!events.empty()) {

        /* find if a server is booting. Assume only one can be booting */
        ModelChangeEvents::const_iterator eventIt = events.begin();
        if (eventIt != events.end()) {
            ASSERT(eventIt->change == SERVER_ONLINE);
            isBooting = true;
            eventIt++;
            ASSERT(eventIt == events.end()); // only one tactic should be active
        }
    }
    return isBooting;
}


Configuration Model::getConfiguration() {
    Configuration configuration;

    configuration.setBrownOutLevel(brownoutFactorToLevel(brownoutFactor));
    configuration.setActiveServers(activeServers);
    if (events.empty()) {
        configuration.setBootRemain(0);
    } else {

        /* find if a server is booting. Assume only one can be booting */
        ModelChangeEvents::const_iterator eventIt = events.begin();
        if (eventIt != events.end()) {
            ASSERT(eventIt->change == SERVER_ONLINE);
            int bootRemain = ceil((eventIt->time - simTime().dbl()) / evaluationPeriod);

            /*
             * we never set boot remain to 0 here because the server could
             * still be booting (if we allowed random boot times)
             * so, we keep it > 0, and only serverBecameActive() can set it to 0
             */
            configuration.setBootRemain(std::max(1, bootRemain));
            eventIt++;
            ASSERT(eventIt == events.end()); // only one tactic should be active
        }
    }
    return configuration;
}

const Environment& Model::getEnvironment() const {
    return environment;
}

void Model::setEnvironment(const Environment& environment) {
    this->environment = environment;
}

int Model::getMaxServers() const {
    return maxServers;
}

double Model::getEvaluationPeriod() const {
    return evaluationPeriod;
}

const Observations& Model::getObservations() const {
    return observations;
}

void Model::setObservations(const Observations& observations) {
    this->observations = observations;
}


double Model::getBootDelay() const {
    return bootDelay;
}

int Model::getHorizon() const {
    return horizon;
}

int Model::getNumberOfBrownoutLevels() const {
    return numberOfBrownoutLevels;
}

int Model::getNumberOfDimmerLevels() const {
    return numberOfBrownoutLevels;
}


double Model::getLowFidelityServiceTime() const {
    return lowFidelityServiceTime;
}

void Model::setLowFidelityServiceTime(double lowFidelityServiceTimeMean, double lowFidelityServiceTimeVariance) {
    this->lowFidelityServiceTime = lowFidelityServiceTimeMean;
    this->lowFidelityServiceTimeVariance = lowFidelityServiceTimeVariance;
}

int Model::getServerThreads() const {
    return serverThreads;
}

void Model::setServerThreads(int serverThreads) {
    this->serverThreads = serverThreads;
}

double Model::getServiceTime() const {
    return serviceTime;
}

void Model::setServiceTime(double serviceTimeMean, double serviceTimeVariance) {
    this->serviceTime = serviceTimeMean;
    this->serviceTimeVariance = serviceTimeVariance;
}

double Model::getLowFidelityServiceTimeVariance() const {
    return lowFidelityServiceTimeVariance;
}

double Model::getServiceTimeVariance() const {
    return serviceTimeVariance;
}

double Model::brownoutLevelToFactor(int brownoutLevel) const {
    if (lowerDimmerMargin) {

        // lower dimmer margin is upper brownout margin
        return (1.0 - dimmerMargin) * (brownoutLevel - 1.0) / (getNumberOfBrownoutLevels() - 1.0);
    }
    return dimmerMargin + (1.0 - 2 * dimmerMargin) * (brownoutLevel - 1.0) / (getNumberOfBrownoutLevels() - 1.0);
}

int Model::brownoutFactorToLevel(double brownoutFactor) const {
    if (lowerDimmerMargin) {

        // lower dimmer margin is upper brownout margin
        return 1 + round(brownoutFactor * (getNumberOfBrownoutLevels() - 1) / (1.0 - dimmerMargin));
    }
    return 1 + round((brownoutFactor - dimmerMargin) * (getNumberOfBrownoutLevels() - 1) / (1.0 - 2 * dimmerMargin));
}


bool Model::isDimmerMarginLower() const {
    return lowerDimmerMargin;
}

double Model::dimmerLevelToFactor(int dimmerLevel) const {
    int brownoutLevel = getNumberOfBrownoutLevels() - dimmerLevel + 1;

    return brownoutLevelToFactor(brownoutLevel);
}

int Model::dimmerFactorToLevel(double dimmerFactor) const {
    return brownoutFactorToLevel(1.0 - dimmerFactor);
}


double Model::getDimmerMargin() const {
    return dimmerMargin;
}
