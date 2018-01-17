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

#ifndef MODEL_H_
#define MODEL_H_

#include <omnetpp.h>
#include <set>
#include "Configuration.h"
#include "Environment.h"
#include "Observations.h"

class Model : public omnetpp::cSimpleModule {
public:

    /*
     * there are some methods (e.g., removeExpectedChange() that do not check
     * the type of change, so if a new change type is added, those methods
     * would have to be fixed
     */
    enum ModelChange { SERVER_ONLINE};

    struct ModelChangeEvent {
        double startTime; // when the event was created
        double time; // when the event will happen
        ModelChange change;
    };

    struct ModelChangeEventComp {
      bool operator() (const ModelChangeEvent& lhs, const ModelChangeEvent& rhs) const {
          return lhs.time < rhs.time;
      }
    };
    typedef std::multiset<ModelChangeEvent, ModelChangeEventComp> ModelChangeEvents;

protected:
    static const char* HORIZON_PAR;
    virtual int numInitStages() const {return 2;}
    virtual void initialize(int stage);

    ModelChangeEvents events;


    // these are used so that we can query the model for what happened an instant earlier
    // TODO save the configuration using a timestamp
    int activeServerCountLast;
    double timeActiveServerCountLast;


    double evaluationPeriod;
    double dimmerMargin; /**< the used range for the dimmer is [dimmerMargin, 1-dimmerMargin] */
    bool lowerDimmerMargin = false;


    // these hold the current configuration, plus the events for the booting server
    int activeServers; /**< number of active servers (there is one more powered up if a server is booting) */
    double brownoutFactor;

    Environment environment;
    Observations observations;
    int maxServers;
    double bootDelay;
    int horizon;
    int serverThreads;
    double serviceTime;
    double serviceTimeVariance;
    double lowFidelityServiceTime;
    double lowFidelityServiceTimeVariance;
    int numberOfBrownoutLevels;

    void addExpectedChange(double time, ModelChange change);

    /**
     * This method removes the last expected change (scheduled farthest in the future)
     * For generality, addExpectedChange() should return some id that could be
     * then used to remove the event.
     */
    void removeExpectedChange();

    bool isServerBooting() const;

public:
    Model();


    /* the following methods are less general */

    /**
     * Returns the expected number of active servers at a time in the future
     */
    int getActiveServerCountIn(double deltaTime);
    int const getActiveServers() const;
    int const getServers() const;

    void addServer(double bootDelay);
    void serverBecameActive();
    void removeServer();

    Configuration getConfiguration();
    const Environment& getEnvironment() const;
    virtual void setEnvironment(const Environment& environment);
    const Observations& getObservations() const;
    void setObservations(const Observations& observations);
    int getMaxServers() const;
    double getEvaluationPeriod() const;
    double getBootDelay() const;
    int getHorizon() const;

    double getLowFidelityServiceTime() const;
    void setLowFidelityServiceTime(double lowFidelityServiceTimeMean, double lowFidelityServiceTimeVariance);
    int getServerThreads() const;
    void setServerThreads(int serverThreads);
    double getServiceTime() const;
    void setServiceTime(double serviceTimeMean, double serviceTimeVariance);

    double getLowFidelityServiceTimeVariance() const;
    double getServiceTimeVariance() const;

    void setDimmerFactor(double factor);
    double getDimmerFactor() const;

    /**
     * Returns true if dimmer margin is only used at the bottom of the range
     */
    bool isDimmerMarginLower() const;

    int getNumberOfDimmerLevels() const;
    double dimmerLevelToFactor(int dimmerLevel) const;
    int dimmerFactorToLevel(double dimmerFactor) const;

    // brownout is the complement of dimmer
    void setBrownoutFactor(double factor);
    double getBrownoutFactor() const;

    int getNumberOfBrownoutLevels() const;
    double brownoutLevelToFactor(int brownoutLevel) const;
    int brownoutFactorToLevel(double brownoutFactor) const;

    double getDimmerMargin() const;

    virtual ~Model();
};

#endif /* MODEL_H_ */
