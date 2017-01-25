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

#ifndef HAPROXYSOCKETCOMMAND_H_
#define HAPROXYSOCKETCOMMAND_H_

#include <sys/un.h>
#include <string>

class HAProxySocketCommand {
    struct sockaddr_un proxyAddress;
public:
    HAProxySocketCommand();
    void setAddress(const std::string& socketPath);

    /**
     * sends command(s) to HAProxy admin socket and returns reply
     */
    std::string executeCommand(const std::string& cmd);

    virtual ~HAProxySocketCommand();
};

#endif /* HAPROXYSOCKETCOMMAND_H_ */
