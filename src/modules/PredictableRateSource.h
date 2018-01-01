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
 * PredictableRateSource.h
 *
 *  Created on: Mar 2, 2015
 *      Author: gmoreno
 */

#ifndef PREDICTABLERATESOURCE_H_
#define PREDICTABLERATESOURCE_H_

#include "PredictableSource.h"

class PredictableRateSource: public PredictableSource {

protected:

  virtual void preload();
};

#endif /* PREDICTABLERATESOURCE_H_ */
