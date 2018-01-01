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

#include "HAProxySocketCommand.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>


#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

using namespace std;

HAProxySocketCommand::HAProxySocketCommand() {
    memset(&proxyAddress, 0, sizeof(proxyAddress));
}

void HAProxySocketCommand::setAddress(const std::string& socketPath) {
    proxyAddress.sun_family = AF_UNIX;
    snprintf(proxyAddress.sun_path, UNIX_PATH_MAX, "%s", socketPath.c_str());
}


std::string HAProxySocketCommand::executeCommand(const std::string& cmd) {
    int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        throw runtime_error("ExecutionManagerHAProxy::sendHAProxyCommand socket() failed");
    }
    if (connect(socket_fd, (struct sockaddr*) &proxyAddress, sizeof(proxyAddress)) != 0) {
        close(socket_fd);
        throw runtime_error("ExecutionManagerHAProxy::sendHAProxyCommand connect() failed");
    }
    ssize_t n = write(socket_fd, cmd.c_str(), cmd.length());
    if (n < (ssize_t) cmd.length()) {
        close(socket_fd);
        throw runtime_error("ExecutionManagerHAProxy::sendHAProxyCommand write() failed");
    }
    char buffer[4096 + 1];
    n = read(socket_fd, buffer, 4096);
    if (n == -1) {
        close(socket_fd);
        throw runtime_error("ExecutionManagerHAProxy::sendHAProxyCommand read() failed");
    }
    buffer[n] = 0;
    return string(buffer);
}


HAProxySocketCommand::~HAProxySocketCommand() {
    // TODO Auto-generated destructor stub
}

