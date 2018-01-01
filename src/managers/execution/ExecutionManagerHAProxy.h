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
#ifndef __PLASASIM_EXECUTIONMANAGERHAPROXY_H_
#define __PLASASIM_EXECUTIONMANAGERHAPROXY_H_

#include "ExecutionManagerModBase.h"
#include "util/HAProxySocketCommand.h"

/**
 * Execution Manager to control HAProxy
 */
class ExecutionManagerHAProxy : public ExecutionManagerModBase
{
    HAProxySocketCommand loadBalancer;
  protected:
    virtual void initialize();


    void disableServer(int serverNumber);

    // target-specific methods (e.g., actual servers, sim servers, etc.)

    /**
     * @return BootComplete* to be handled later by doAddServerBootComplete()
     */
    virtual BootComplete* doAddServer(bool instantaneous = false);
    virtual void doAddServerBootComplete(BootComplete* bootComplete);

    /**
     * @return BootComplete* identical in content (not the pointer itself) to
     *   what doAddServer() would have return for this server
     */
    virtual BootComplete* doRemoveServer();
    virtual void doSetBrownout(double factor);
};

#endif
