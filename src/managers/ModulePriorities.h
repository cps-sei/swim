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
#ifndef MODULEPRIORITIES_H
#define MODULEPRIORITIES_H

/*
 *  by default all messages, such as boot complete, are set at priority 0 (highest)
 *  the rest are order in decreasing priority (increasing value)
 */

/* this is used for msgs that control monitoring done at higher rate than eval period (e.g., KF update) */
#define MONITOR_PRE_OVERSAMPLING_PRIO 1

/* this is used for collecting all measurements before adapt mgr decided */
#define MONITOR_PRE_PRIO 2

/* this is for the adaptation mgr */
#define ADAPTATION_MGR_PRIO 3

/* this is for emitting signals reflecting actions just taken by adapt mgr */
#define MONITOR_POST_PRIO 4

#endif
