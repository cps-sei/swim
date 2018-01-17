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
#ifndef SWIMCLIENT_H_
#define SWIMCLIENT_H_

#include <stdexcept>
#include <string>
#include <boost/asio.hpp>

class SwimClient {
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::tcp::socket socket;

    std::string sendCommand(const char* command);
    double probeDouble(const char* command);
    int probeInt(const char* command);

public:
    SwimClient();
    void connect(const char* host, const char* port = "4242");
    bool isConnected() const;

    // probes
    double getDimmer();
    int getServers();
    int getActiveServers();
    int getMaxServers();
    double getUtilization(int serverId);
    double getBasicResponseTime();
    double getOptionalResponseTime();
    double getBasicThroughput();
    double getOptionalThroughput();
    double getArrivalRate();

    // effectors
    void addServer();
    void removeServer();
    void setDimmer(double dimmer);

    // helper methods
    double getTotalUtilization();
    double getAverageResponseTime();

    virtual ~SwimClient();
};

#endif /* SWIMCLIENT_H_ */
