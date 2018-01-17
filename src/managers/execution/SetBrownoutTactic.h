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

#ifndef SETBROWNOUTTACTIC_H_
#define SETBROWNOUTTACTIC_H_

#include "Tactic.h"

class SetBrownoutTactic: public Tactic {
    double factor;
public:
    SetBrownoutTactic(double factor);
    virtual void execute(ExecutionManager* execMgr);
    virtual void printOn(std::ostream& os) const;
    virtual ~SetBrownoutTactic();
};

#endif /* SETBROWNOUTTACTIC_H_ */
