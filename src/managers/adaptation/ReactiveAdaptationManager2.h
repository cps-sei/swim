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

#ifndef __PLASASIM_REACTIVEADAPTATIONMANAGER2_H_
#define __PLASASIM_REACTIVEADAPTATIONMANAGER2_H_

#include "BaseAdaptationManager.h"

/**
 * Simple reactive adaptation manager (alternative 2)
 */
class ReactiveAdaptationManager2 : public BaseAdaptationManager
{
  protected:
    virtual Tactic* evaluate();
};

#endif
