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

#include "TimeWindowStats.h"
#include <iostream>

using namespace std;

TimeWindowStats::TimeWindowStats() {
    setWindow(60);
}

TimeWindowStats::~TimeWindowStats() {
    // TODO Auto-generated destructor stub
}

void TimeWindowStats::reset() {
    entries.clear();
    pStats.reset(nullptr);
}

void TimeWindowStats::setWindow(unsigned seconds) {
#if USE_OMNET_CLOCK
    window = seconds;
#else
    window = std::chrono::seconds(seconds);
#endif
}


void TimeWindowStats::record(double value) {
    if (entries.size() > ENTRIES_BETWEEN_CHECKS) {
        removeOldEntries();
    }
    pStats.reset(nullptr);
    entries.push_back(Entry { getTimestampNow(), value });
    lastValue = value;
}

/**
 * Loads pStat with all the entries only if pStats == nullptr
 *
 * pStats is reset to nullptr every time the list of valid entries changes
 */
void TimeWindowStats::loadStats() {
    if (pStats == nullptr) {
        pStats.reset(new Stats);
        for (const auto& entry : entries) {
            pStats->operator ()(entry.value);
        }
    }
}

double TimeWindowStats::getAverage() {
    removeOldEntries();
    double avg = 0.0;

    if (entries.size() > 0) {
        loadStats();
        avg = boost::accumulators::mean(*pStats);
    }

    return avg;
}

double TimeWindowStats::getVariance() {
    removeOldEntries();
    double variance = 0.0;

    if (entries.size() > 0) {
        loadStats();
        variance = boost::accumulators::moment<2>(*pStats);
    }

    return variance;
}

double TimeWindowStats::getRate() {
    removeOldEntries();

    // TODO this works only for OMNeT++
    return double(entries.size()) / min(asDouble(window), omnetpp::simTime().dbl());
}

unsigned TimeWindowStats::getCount() {
    removeOldEntries();
    return entries.size();
}

double TimeWindowStats::getPercentageAboveZero() {
    removeOldEntries();
    if (entries.empty()) {
        return (lastValue > 0) ? 1.0 : 0.0;
    }

    auto windowStart = getTimestampNow() - window;
    Duration busyTime(0);
    auto lastBusyPeriodStart = windowStart; // assume this for now
    double level = -1.0;
    for (const auto& entry : entries) {
        if (level < 0 && entry.value == 0) { // busy period ends

            /*
             * first entry goes to 0, so assume that at the beginning of
             * the window, it was > 0
             */
            busyTime += entry.timestamp - windowStart;
        } else if (level > 0.0 && entry.value == 0.0) { // busy period ends
            busyTime += entry.timestamp - lastBusyPeriodStart;
        } else if (level <= 0.0 && entry.value > 0.0) { // busy period starts
            lastBusyPeriodStart = entry.timestamp;
        }
        level = entry.value;
    }

    if (level > 0) {

        /* busy period hasn't finished */
        busyTime += getTimestampNow() - lastBusyPeriodStart;
    }

    return asDouble(busyTime) / asDouble(window);
}

void TimeWindowStats::removeOldEntries() {
    bool entriesRemoved = false;
    auto windowStart = getTimestampNow() - window;
    while (!entries.empty() && entries.front().timestamp < windowStart) {
        entries.pop_front();
        entriesRemoved = true;
    }

    if (entriesRemoved) {
        pStats.reset(nullptr);
    }
}

