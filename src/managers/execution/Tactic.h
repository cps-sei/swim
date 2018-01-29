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
 * Tactic.h
 *
 *  Created on: Jan 3, 2015
 *      Author: gmoreno
 */

#ifndef TACTIC_H_
#define TACTIC_H_

#include <managers/execution/ExecutionManager.h>
#include <ostream>

class Tactic {
public:
    virtual void execute(ExecutionManager* execMgr) = 0;

    /**
     * Indicates if a tactic is empty.
     *
     * The only tactic that can be empty is a MacroTactic
     */
    virtual bool isEmpty() const;


    /**
     * Print tactic info
     *
     * @param os output stream
     */
    virtual void printOn(std::ostream& os) const = 0;
    virtual ~Tactic();

    friend std::ostream& operator<<(std::ostream& os, const Tactic& tactic);
};

#endif /* TACTIC_H_ */
