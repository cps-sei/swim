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

#ifndef MACROTACTIC_H_
#define MACROTACTIC_H_

#include "Tactic.h"
#include <list>

class MacroTactic: public Tactic {
    typedef std::list<Tactic*> TacticList;
    TacticList tactics;
public:
    /**
     * Adds a tactic to the macro.
     *
     * The MacroTactic takes over the control of the object pointed to by
     * pTactic.
     */
    void addTactic(Tactic* pTactic);
    bool isEmpty() const;

    /**
     * If it has only one tactic
     */
    bool isSingle() const;

    virtual void execute(ExecutionManager* execMgr);
    virtual void printOn(std::ostream& os) const;
    virtual ~MacroTactic();
};

#endif /* MACROTACTIC_H_ */
