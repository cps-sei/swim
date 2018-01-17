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
 * AddServerTactic.h
 *
 *  Created on: Jan 3, 2015
 *      Author: gmoreno
 */

#ifndef ADDSERVERTACTIC_H_
#define ADDSERVERTACTIC_H_

#include "Tactic.h"

class AddServerTactic: public Tactic {
public:
    virtual void execute(ExecutionManager* execMgr);
    virtual void printOn(std::ostream& os) const;
    virtual ~AddServerTactic();
};

#endif /* ADDSERVERTACTIC_H_ */
