###############################################################################
# Simulator of Web Infrastructure and Management
# Copyright (c) 2016 Carnegie Mellon University.
# All Rights Reserved.
#  
# THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
# MELLON UNIVERSITY EXPRESSLY DISCLAIMS TO THE FULLEST EXTENT PERMITTED BY LAW
# ALL EXPRESS, IMPLIED, AND STATUTORY WARRANTIES, INCLUDING, WITHOUT
# LIMITATION, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
#  
# Released under a BSD license, please see license.txt for full terms.
# DM-0003883
###############################################################################

# computes the average of an observed metric for each period
# df is a data.frame with columns x and y, where x is the time and y is the observation
# the resulting average is for the period ending at time x
periodicAverage <- function(df, period) {
  start <- floor(min(df$x) / period) * period
  end <- ceiling(max(df$x) / period) * period
  intervals <- seq(start + period, end, period)
  
  avg <- cbind(intervals,
                       tapply(df$y, 
                              findInterval(df$x, intervals),
                              mean))
  #quantile, probs=c(0.95))) # to get the 95% quantile
  colnames(avg) <- c("x", "y")
  as.data.frame(avg)
}

# computes the time-weighted average of an observed metric for each period
# df is a data.frame with columns x and y, where x is the time and y is the observation
# the resulting average is for the period starting at time x
timeWeightedAverage <- function(df, period) {
  start <- floor(min(df$x) / period) * period
  end <- ceiling(max(df$x) / period) * period
  end <- ifelse(max(df$x)==end, end+period, end)
  intervals <- seq(start, end, period)

 # if an interval starts without an entry, we need to add one
 # representing the starting value, which is the last observation before the
 # start of the interval
 missing <- intervals[!(intervals %in% df$x)]
 df <- rbind(df, cbind(x=missing,
                       y=apply(as.array(missing), 1, function(t) { df[rev(order(df$x < t))[1], 2]})))
 
 # sort by time
 df <- df[order(df$x),]
  
  # compute the end of the interval each observation belongs to
  endOfInterval <- intervals[findInterval(df$x, intervals)] + period
  
  # expand df with weight column (for how long the obsevation was valid in the period)
  expdf <- cbind(df[1:nrow(df)-1,], 
                 weight=pmin(df$x[2:nrow(df)], endOfInterval[1:nrow(df)-1]) - df$x[1:nrow(df)-1])

  # compute weighted mean
  wm <- do.call("rbind", 
                as.list(by(expdf, findInterval(expdf$x, intervals),
                           function(x) {weighted.mean(x$y, x$weight)})))
  result <- as.data.frame(cbind(x=intervals[1:length(intervals)-1], y=wm))
  colnames(result) <- c('x','y')
  result
}

# utility function used for
# Gabriel A. Moreno, Javier Camara, David Garlan and Bradley Schmerl.
#   "Efficient Decision-Making under Uncertainty for Proactive Self-Adaptation"
#   Proc. of the International Conference on Autonomic Computing (2016)
#   http://works.bepress.com/gabriel_moreno/28/ 
periodUtilityICAC2016 <- function(maxServers, maxServiceRate, arrivalRateMean, dimmer,
                          evaluationPeriod, RT_THRESHOLD, avgResponseTime,
                          avgServers) {
  basicRevenue <- 1
  optRevenue <- 1.5
  maxThroughput <- maxServers * maxServiceRate
  latePenalty <- maxThroughput * optRevenue
  
  positiveUtility <- round((arrivalRateMean * ((1 - dimmer) * basicRevenue + dimmer * optRevenue)))

  utility <- ifelse(avgResponseTime>RT_THRESHOLD | avgResponseTime < 0,
         pmin(0.0, arrivalRateMean * optRevenue - latePenalty),
         positiveUtility)

}

# utility function used for
# Gabriel A. Moreno, Alessandro V. Papadopoulos, Konstantinos Angelopoulos, Javier Camara, et al.
#   "Comparing model-based predictive approaches to self-adaptation: CobRA and PLA"
#   Proceedings of the 12th International Symposium on Software Engineering for Adaptive and Self-Managing Systems (SEAMS) (2017)
#   http://works.bepress.com/gabriel_moreno/33/
periodUtilitySEAMS2017A <- function(maxServers, maxServiceRate, arrivalRateMean, dimmer,
                                  evaluationPeriod, RT_THRESHOLD, avgResponseTime,
                                  avgServers) {
  basicRevenue <- 1
  optRevenue <- 1.5
  serverCost <- 10
  
  precision <- 1e-5
  
  maxThroughput <- maxServers * maxServiceRate

  Ur <- (arrivalRateMean * ((1 - dimmer) * basicRevenue + dimmer * optRevenue))
  Uc <- serverCost * (maxServers - avgServers)
  UrOpt <- arrivalRateMean * optRevenue
  
  utility <- ifelse(avgResponseTime <= RT_THRESHOLD & Ur >= UrOpt - precision, Ur + Uc,
                    ifelse(avgResponseTime <= RT_THRESHOLD, Ur,
                           pmin(0.0, arrivalRateMean - maxThroughput) * optRevenue))
}

readVector <- function(dbconn, vectorName, moduleName) {
    query <- paste("SELECT simtimeRaw/1e12 as x, CAST(value as REAL) as y FROM vector NATURAL JOIN vectorData",
                  " WHERE vectorName='", vectorName, "'", sep="")
    if (!missing(moduleName)) {
      query <- paste(query, " AND moduleName='", moduleName, "'", sep="")
    }
    dbGetQuery(dbconn, query)  
}

plotResults <- function(config, folder="SWIM", run, saveAs=NULL, instantaneousUtility=FALSE,
                         periodGrid=FALSE, utilityFc=periodUtilitySEAMS2017A,
                         brief=FALSE) {
  USE_COMPUTED_UTILITY <- TRUE
  require(reshape2)
  require(ggplot2)
  require(cowplot)
  require(scales)
  require(RSQLite)
  
  basedir <- "./"

  lineWidth <- 1 # full page

  scalarDBPath <- paste(basedir, folder, "/", config, "-", run, ".sca", sep="")
  sdb <- dbConnect(RSQLite::SQLite(), scalarDBPath)
  scalars <- dbReadTable(sdb, "scalar")

  # get network name
  network = scalars[scalars$scalarName=="maxServers", "moduleName"]
  
  # get scalars
  bootDelay = scalars[scalars$scalarName=="bootDelay", "scalarValue"]
  evaluationPeriod = scalars[scalars$scalarName=="evaluationPeriod", "scalarValue"]
  RT_THRESHOLD_SEC = scalars[scalars$scalarName=="responseTimeThreshold", "scalarValue"]  
  maxServers = scalars[scalars$scalarName=="maxServers", "scalarValue"]  
  maxServiceRate = scalars[scalars$scalarName=="maxServiceRate", "scalarValue"]  

  # get vectors
  vectorDBPath <- paste(basedir, folder, "/", config, "-", run, ".vec", sep="")
  vdb <- dbConnect(RSQLite::SQLite(), vectorDBPath)
  
  servers <- readVector(vdb, "serverCost:vector")
  activeServers <- readVector(vdb, "activeServers:vector")
  dimmer <- transform(readVector(vdb, "brownoutFactor:vector"), y = 1 - y)
  responses <- readVector(vdb, "lifeTime:vector") # this gets both low and high
  
  lowResponses <- readVector(vdb, "lifeTime:vector", moduleName=paste(network,".sinkLow", sep=""))
  pctLow <- 100 * length(lowResponses$y) / length(responses$y)
  pctLate <- 100 * sum(responses$y > RT_THRESHOLD_SEC) / length(responses$y)
  avgInterarrival <- periodicAverage(readVector(vdb, "interArrival:vector"), evaluationPeriod)
  avgArrivalRate <- transform(avgInterarrival, y = 1 / y)
  
  start <- floor(min(servers$x) / evaluationPeriod) * evaluationPeriod
  end <- ceiling(max(servers$x) / evaluationPeriod) * evaluationPeriod
  
  # find the mean for the response time in each interval
  avgresponse <- periodicAverage(responses, evaluationPeriod)
  
  
  if (USE_COMPUTED_UTILITY) {
    cat(paste("Computing utility with", deparse(substitute(utilityFc))), "\n")
    
    # compute the weighted mean for brownout factor
    dimmerMean <- timeWeightedAverage(dimmer, evaluationPeriod)
    
    # shift them on period so that they align with the the other vectors
    # that is, observations for the period ending
    dimmerMean$x = dimmerMean$x + evaluationPeriod
    
    # compute the weighted mean for the servers
    serversMean <- timeWeightedAverage(servers, evaluationPeriod)
    
    # shift them on period so that they align with the the other vectors
    # that is, observations for the period ending
    serversMean$x = serversMean$x + evaluationPeriod
    
    # trim all datasets
    avgArrivalRate <- subset(avgArrivalRate, x <= end)
    dimmerMean <- subset(dimmerMean, x <= end)
    serversMean <- subset(serversMean, x <= end)
    avgresponse <- subset(avgresponse, x <= end)    
    
    utility <- as.data.frame(cbind(x=avgresponse$x,
                                   y=utilityFc(maxServers, maxServiceRate,
                                                   avgArrivalRate$y, dimmerMean$y,
                                                   evaluationPeriod, RT_THRESHOLD_SEC,
                                                   avgresponse$y, serversMean$y)))
    totalUtility <- sum(utility$y)
  } else {
    utility <- readVector(vdb, "utilityPeriod:vector")
    totalUtility <- scalars[scalars$scalarName=="utility:last", "scalarValue"]
  }

  # melt data
  tmp <-melt(avgArrivalRate, id=c("x"))
  tmp$variable='requestRate'
  md <- tmp
  
  tmp <-melt(avgresponse, id=c("x"))
  tmp$variable='responseTime'
  md <- rbind(md, tmp)
  
  tmp <-melt(servers, id=c("x"))
  tmp$variable='servers'
  md <- rbind(md, tmp)
  
  tmp <-melt(activeServers, id=c("x"))
  tmp$variable='activeServers'
  md <- rbind(md, tmp)
  
  tmp <-melt(dimmer, id=c("x"))
  tmp$variable='dimmer'
  md <- rbind(md, tmp)
  
  tmp <-melt(utility, id=c("x"))
  tmp$variable='utility'
  md <- rbind(md, tmp)
  
  names(md)[1] <- 'time'
  
  # this is to have the factors in the order they were added
  # important for ploting activeServers over servers
  md$variable <- factor(md$variable, unique(as.character(md$variable)))
  
  pltRequests <- ggplot(md, aes(x=time,y=value)) +
    geom_line(data=subset(md, variable=="requestRate")) +
    ylab('requests/s')
  pltServers <- ggplot(md, aes(x=time,y=value,linetype=variable)) +
    geom_step(data=subset(md, variable %in% c("servers", "activeServers"))) +
    scale_linetype_manual(labels=c('servers','active servers'), values=c("dashed", "solid")) +
    scale_y_continuous(breaks=seq(0,maxServers)) + 
    expand_limits(y=0) + 
    ylab('servers')
  pltDimmer <- ggplot(md, aes(x=time,y=value)) + 
    geom_step(data=subset(md, variable=="dimmer")) + 
    ylab('dimmer') + expand_limits(y=c(0,1))
  pltResponse <- ggplot(md, aes(x=time,y=value)) + 
    geom_line(data=subset(md, variable=="responseTime")) + 
    ylab('resp. time (s)') + 
    geom_hline(yintercept=RT_THRESHOLD_SEC, linetype="dashed", color="darkgrey")
  
  if (instantaneousUtility) {
    pltInstUtility <- ggplot(md, aes(x=time,y=value)) + 
      geom_line(data=subset(md, variable=="utility")) + 
      ylab('utility')
  }
  
  pltUtility <- ggplot(md, aes(x=time,y=cumsum(value))) + 
    geom_line(data=subset(md, variable=="utility")) + 
    ylab('cum. utility')
  
  # this computes the avg of the amount of RT excess over the threshold over the whole run
  avgError <- sum(pmax(avgresponse$y - RT_THRESHOLD_SEC, 0)) / (length(avgresponse$y) * evaluationPeriod)
  
  overshoot <- max(avgresponse$y - RT_THRESHOLD_SEC, 0)
  
  tableHeader <- paste("Config & Utility & \\% optional & \\% late & avg. servers & avg. error & overshoot\\\\\n")
  tableRow <- paste(config, "&", round(totalUtility), "&", round(100-pctLow, 1),
                    "&", round(pctLate,1), "&", round(mean(servers$y), 1), "&", avgError, "&", overshoot, "\\\\\n")
  cat(tableHeader, tableRow)

  theme_set(theme_cowplot(font_size = 8))

  if (brief) {
    plotList <- list(pltRequests, pltServers, pltDimmer, pltResponse)
    relHeights <- c(1,1,1,1.25)
  } else if(instantaneousUtility) {
    plotList <- list(pltRequests, pltServers, pltDimmer, pltResponse, pltInstUtility, pltUtility)
    relHeights <- c(1,1,1,1,1,1.15)
  } else {
    plotList <- list(pltRequests, pltServers, pltDimmer, pltResponse, pltUtility)
    relHeights <- c(1,1,1,1,1.2)
  }
  
  # apply uniform formatting
  periodicBreaks <- function(a, b, period, n) {
    seq(a, ceiling(b/period)*period, floor((b-a)/(period*n)) * period)
  }
  
  commonFormat <- function(p) { 
    rp <- p + expand_limits(x=start) +
      background_grid() +
      theme(legend.position="none")
    if (periodGrid) {
        rp + scale_x_continuous(breaks=periodicBreaks(start,end,evaluationPeriod,10),
                                minor_breaks = seq(start, end, evaluationPeriod))
    } else {
        rp + scale_x_continuous(breaks=pretty_breaks())
    }
  }

  # for all but the bottom chart
  topFormat <- function(p) {
    p + theme(axis.text.x = element_blank(),
          axis.ticks = element_blank(),
          axis.title.x = element_blank())
  }
  
  # for the bottom chart
  bottomFormat <- function(p) {
    p + xlab("time (s)")
  }
  
  for (i in seq(1,length(plotList)-1)) {
    plotList[[i]] <- commonFormat(topFormat(plotList[[i]]))
  }
  plotList[[length(plotList)]] <- commonFormat(bottomFormat(plotList[[length(plotList)]]))
  
  plotGrid <- plot_grid(plotlist=plotList,
                          ncol=1, align = "v", rel_heights = relHeights)

  if (!is.null(saveAs)) {
    save_plot(saveAs, plotGrid, ncol = 1, nrow=length(plotList), base_height = 1, base_aspect_ratio = 4)
  }
  
  dbDisconnect(sdb)
  dbDisconnect(vdb)
  
  plotGrid
}
