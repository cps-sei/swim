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

package plasa.modules;

import org.omnetpp.queueing.PassiveQueue;


//
// PassiveQueue that works with dynamically connected servers
//
simple PassiveQueueDyn extends PassiveQueue
{
    parameters:
        @signal[interArrival](type="double");
        @statistic[interArrival](record=vector);
    
    @class(PassiveQueueDyn);
}
