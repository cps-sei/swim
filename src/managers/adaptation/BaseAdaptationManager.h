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

#ifndef __PLASA_BASEADAPTATIONMANAGER_H_
#define __PLASA_BASEADAPTATIONMANAGER_H_

#include <omnetpp.h>
#include "model/Model.h"
#include "managers/execution/Tactic.h"

/**
 * Base class for all adaptation manager modules
 */
class BaseAdaptationManager : public omnetpp::cSimpleModule
{
    omnetpp::simsignal_t decisionTimeSignal;
    Model* pModel;

protected:
    omnetpp::cMessage *periodEvent; // pointer to the event object which we'll use for timing
    omnetpp::cMessage *decisionCompleteEvent; // event for notifying of decision completion
  Tactic* pTactic;

  virtual int numInitStages() const {return 2;}
  virtual void initialize(int stage);
  virtual void handleMessage(omnetpp::cMessage *msg);

  /**
   * Dispatches the execution of the result of the decision
   */
  virtual void decisionComplete();

  inline Model* getModel() { return pModel; }

  /**
   * Adaptation decision implementation
   *
   * @return Tactic* (possibly a MacroTactic*) or null if none must be executed.
   *   Deletion of the tactic object is done by the caller of this method.
   */
  virtual Tactic* evaluate() = 0;

public:
  BaseAdaptationManager();
  virtual ~BaseAdaptationManager();
};

#endif
