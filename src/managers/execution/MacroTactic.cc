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

#include "MacroTactic.h"

void MacroTactic::addTactic(Tactic* pTactic) {
    tactics.push_back(pTactic);
}

void MacroTactic::execute(ExecutionManager* execMgr) {
    for (TacticList::iterator it = tactics.begin(); it != tactics.end(); ++it) {
        (*it)->execute(execMgr);
    }
}

bool MacroTactic::isEmpty() const {
    for (TacticList::const_iterator it = tactics.begin(); it != tactics.end(); ++it) {
        if (!(*it)->isEmpty()) {
            return false;
        }
    }
    return true;
}

bool MacroTactic::isSingle() const {
    return tactics.size() == 1;
}

MacroTactic::~MacroTactic() {
    for (TacticList::iterator it = tactics.begin(); it != tactics.end(); ++it) {
        delete (*it);
    }
}

void MacroTactic::printOn(std::ostream& os) const {
    os << "Macro[";
    for (TacticList::const_iterator it = tactics.begin(); it != tactics.end(); ++it) {
        if (it != tactics.begin()) {
            os << ", ";
        }
        (*it)->printOn(os);
    }
    os << "]";
}
