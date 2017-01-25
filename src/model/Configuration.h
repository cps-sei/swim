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
#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <pladapt/Configuration.h>
#include <ostream>

class Configuration : public pladapt::Configuration {
    int servers; // number of active servers (there is one more powered up if bootRemain > 0
    int bootRemain; // how many periods until we have one more server. If 0, no server is booting
    int brownoutLevel;
    bool coldCache; // true if the cache of the last added server is cold
public:
    Configuration();
    Configuration(int servers, int bootRemain, int brownoutLevel, bool coldCache);

//    virtual bool operator==(const Configuration& other) const;
//    virtual void printOn(std::ostream& os) const;
//    virtual unsigned poweredServers() const;
//
//    friend std::ostream& operator<<(std::ostream& os, const Configuration& config);


    int getBootRemain() const;
    void setBootRemain(int bootRemain);
    int getBrownOutLevel() const;
    void setBrownOutLevel(int brownoutLevel);
    int getActiveServers() const;
    void setActiveServers(int servers);
    int getServers() const;
    bool isColdCache() const;
    void setColdCache(bool coldCache);

    virtual void printOn(std::ostream& os) const;

protected:
    virtual bool equals(const pladapt::Configuration& other) const;
};

#endif /* CONFIGURATION_H_ */
