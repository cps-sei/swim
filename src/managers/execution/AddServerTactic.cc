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
 * AddServerTactic.cpp
 *
 *  Created on: Jan 3, 2015
 *      Author: gmoreno
 */

#include "AddServerTactic.h"

void AddServerTactic::execute(ExecutionManager* execMgr) {
    execMgr->addServer();
}

void AddServerTactic::printOn(std::ostream& os) const {
    os << "AddServer";
}

AddServerTactic::~AddServerTactic() {
    // TODO Auto-generated destructor stub
}

