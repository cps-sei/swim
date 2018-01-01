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
#include "Configuration.h"
#include <typeinfo>

//    int brownoutLevel = 1 + (pModel->getNumberOfBrownoutLevels() - 1) * pModel->getConfiguration().getBrownOutFactor();


Configuration::Configuration() : servers(0), bootRemain(0), brownoutLevel(0), coldCache(false) {}

Configuration::Configuration(int servers, int bootRemain, int brownoutLevel, bool coldCache)
    :  servers(servers),  bootRemain(bootRemain),  brownoutLevel(brownoutLevel),
        coldCache(coldCache) {};


int Configuration::getBootRemain() const {
    return bootRemain;
}

int Configuration::getServers() const {
    return servers + ((bootRemain > 0) ? 1 : 0);
}

void Configuration::setBootRemain(int bootRemain) {
    this->bootRemain = bootRemain;
}

int Configuration::getBrownOutLevel() const {
    return brownoutLevel;
}

void Configuration::setBrownOutLevel(int brownoutLevel) {
    this->brownoutLevel = brownoutLevel;
}

int Configuration::getActiveServers() const {
    return servers;
}

void Configuration::setActiveServers(int servers) {
    this->servers = servers;
    this->bootRemain = 0;
}

bool Configuration::isColdCache() const {
    return coldCache;
}

void Configuration::setColdCache(bool coldCache) {
    this->coldCache = coldCache;
}

bool Configuration::equals(const pladapt::Configuration& other) const {
    try {
        const Configuration& otherConf = dynamic_cast<const Configuration&>(other);
        return servers == otherConf.servers && brownoutLevel == otherConf.brownoutLevel
                && bootRemain == otherConf.bootRemain && coldCache == otherConf.coldCache;
    }
    catch(std::bad_cast&) {}
    return false;
}

void Configuration::printOn(std::ostream& os) const {
    os << "config[servers=" << servers << ", bootRemain=" << bootRemain
            << ", brownoutLevel=" << brownoutLevel << ", coldCache=" << coldCache << "]";
}

