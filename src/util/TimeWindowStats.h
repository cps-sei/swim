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

#ifndef TIMEWINDOWSTATS_H_
#define TIMEWINDOWSTATS_H_

#include <deque>
#include <memory>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>

#define USE_OMNET_CLOCK 1

#if USE_OMNET_CLOCK
#include <omnetpp.h>
#else
#include <chrono>
#endif

/**
 * Computes statistics for events in a sliding time window
 *
 * @note This class is not thread-safe (intended for use in OMNET++)
 */
class TimeWindowStats {
public:
    TimeWindowStats();
    virtual ~TimeWindowStats();

    virtual void reset();
    virtual void setWindow(unsigned seconds);
    virtual void record(double value);
    virtual double getAverage();
    virtual double getVariance();

    /**
     * Returns the average number of entries per second
     */
    virtual double getRate();
    virtual unsigned getCount();

    /**
     * Assuming that entries set the new level for a continuous signal,
     * it computes the percentage of time the signal was above 0.
     *
     * TODO this probably should be a specialization for boolean values
     */
    virtual double getPercentageAboveZero();

protected:
    typedef boost::accumulators::accumulator_set<double,
            boost::accumulators::stats<boost::accumulators::tag::mean,
                    boost::accumulators::tag::moment<2> > > Stats;
    std::unique_ptr<Stats> pStats;

    void loadStats();

#if USE_OMNET_CLOCK
    using Duration = omnetpp::simtime_t;
    using Timestamp = omnetpp::simtime_t;
#else
    using clock = std::chrono::system_clock;
    using Duration = clock::duration;
    using Timestamp = clock::time_point;
#endif

    /**
     * Window duration
     */
    Duration window;

    struct Entry {
        Timestamp timestamp;
        double value;
    };

    std::deque<Entry> entries;

    const unsigned ENTRIES_BETWEEN_CHECKS = 2000;
    double lastValue = 0; /**< keeps the last value even if it falls out of the window */

    /**
     * Removes the entries that fell out of the time window
     */
    void removeOldEntries();

    inline Duration getDuration(Timestamp first, Timestamp last) const {
        return last - first;
    }

    inline Timestamp subtractDuration(Timestamp t, Duration d) const {
        return t - d;
    }

#if USE_OMNET_CLOCK
    inline Timestamp getTimestampNow() const {
        return omnetpp::simTime();
    }

    inline double asDouble(Duration t) const {
        return t.dbl();
    }
#else
    inline Timestamp getTimestampNow() const {
        return clock::now();
    }

    inline double asDouble(Duration t) const {
        return std::chrono::duration<double>(t).count();
    }
#endif

};

#endif /* TIMEWINDOWSTATS_H_ */
