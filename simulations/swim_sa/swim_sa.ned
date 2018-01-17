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

package plasa.simulations.swim_sa;

import org.omnetpp.queueing.Classifier;
import org.omnetpp.queueing.Router;
import org.omnetpp.queueing.Source;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.SourceOnce;
import plasa.managers.monitor.SimpleMonitor;
import plasa.managers.execution.ExecutionManager;
import plasa.managers.adaptation.IAdaptationManager;
import plasa.managers.monitor.SimProbe;
import plasa.model.Model;
import plasa.modules.AppServer;
import plasa.modules.ArrivalMonitor;
import plasa.modules.PredictableSource;
import plasa.modules.PredictableRandomSource;



network SWIM_SA
{
    parameters:
        double bootDelay = default(0);
        double evaluationPeriod = default(10);
        int initialServers = default(1);
        int maxServers = default(1);
        int numberOfBrownoutLevels;
        string adaptationManagerType;
        double dimmerMargin = default(0.0);
        double responseTimeThreshold @unit(s) = default(1s);
        double maxServiceRate;
        double optRevenue = default(1.5);
        double penaltyMultiplier = default(1);

    submodules:
        sink: Sink {
            @display("p=522,211");
        }
        loadBalancer: Router {
            @display("p=302,159");
            routingAlgorithm = "roundRobin";
        }
        executionManager: ExecutionManager {
            @display("p=85,53");
        }
        adaptationManager: <adaptationManagerType> like IAdaptationManager {
            @display("p=329,53");
        }
        model: Model {
            @display("p=201,53");
        }
        monitor: SimpleMonitor {
            @display("p=501,73");
        }
        arrivalMonitor: ArrivalMonitor {
            @display("p=187,152");
        }
        source: PredictableSource {
            @display("p=54,165");
        }
        classifier: Classifier {
            @display("p=431,165");
        }
        sinkLow: Sink {
            @display("p=522,142");
        }
        probe: SimProbe {
            @display("p=439,28");
        }
    connections:
        arrivalMonitor.out --> loadBalancer.in++;
        source.out --> arrivalMonitor.in;
        probe.out++ --> monitor.probe;
        classifier.out++ --> sink.in++;
        classifier.rest --> sinkLow.in++;
}
