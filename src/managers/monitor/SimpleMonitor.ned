//******************************************************************************
// Simulator of Web Infrastructure and Management
// Copyright (c) 2016 Carnegie Mellon University.
// All Rights Reserved.
//  
// THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
// MELLON UNIVERSITY EXPRESSLY DISCLAIMS TO THE FULLEST EXTENT PERMITTED BY LAW
// ALL EXPRESS, IMPLIED, AND STATUTORY WARRANTIES, INCLUDING, WITHOUT
// LIMITATION, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
//  
// Released under a BSD license, please see license.txt for full terms.
// DM-0003883
//******************************************************************************

package plasa.managers.monitor;

//
// TODO auto-generated module
//
simple SimpleMonitor
{
    parameters:
        int oversamplingFactor = default(1); // note that a value != 1, it only makes sense if an OS predictor is used

        @signal[numberOfServers](type="long");
        @signal[activeServers](type="long");
        @statistic[serverCost](source=numberOfServers; record=timeavg,vector);
        @statistic[activeServers](record=vector);
        
        @signal[brownoutFactor](type="double");
        @statistic[brownoutFactor](record=vector);
        
        @signal[utility](type="double");
        @statistic[utility](source="sum(utility)"; record=last);
        @statistic[utilityPeriod](source="utility"; record=vector);
        
		// inspection
 		@signal[measuredInterarrivalAvg](type="double");
 		@signal[measuredInterarrivalStdDev](type="double");
        @statistic[measuredInterarrivalAvg](record=vector);
        @statistic[measuredInterarrivalStdDev](record=vector);
        @signal[estimatedBasicServiceTime](type="double");
        @statistic[estimatedBasicServiceTime](record=vector);
        @signal[estimatedOptServiceTime](type="double");
        @statistic[estimatedOptServiceTime](record=vector);
        @signal[estimatedBkgUtilization](type="double");
        @statistic[estimatedBkgUtilization](record=vector);
        
    gates:
        input probe;
}
