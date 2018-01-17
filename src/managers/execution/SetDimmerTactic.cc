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

#include "SetDimmerTactic.h"

SetDimmerTactic::SetDimmerTactic(double factor) : factor(factor) {
}

void SetDimmerTactic::execute(ExecutionManager* execMgr) {
    execMgr->setBrownout(1.0 - factor);
}

void SetDimmerTactic::printOn(std::ostream& os) const {
    os << "SetDimmer(" << factor << ")";
}

SetDimmerTactic::~SetDimmerTactic() {
}

