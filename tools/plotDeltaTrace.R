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
plotDeltaTrace <- function(path, numIntervals, title="") {
  tr <-
    read.table(
      path,
      sep = " ",
      col.names = c("delta"),
      quote = "\"",
      fill = TRUE,
      nrows = -1
    )
  
  d <- tr$delta
  t <- cumsum(d)
  #t <- t[t >= 7200]
  d <- d[(1 + length(d) - length(t)):length(d)]
  #intervals <- c(360,120,60,30)
  #intervals <- c(sum(d) / 60)
  intervals <- c(numIntervals)
  for (n in seq(1, length(intervals))) {
    m <- tapply(d, cut(t, intervals[n], labels = FALSE)[1:length(d)], mean)
    s <- max(intervals) / intervals[n]
    f <- (1 + s) / 2
    if (n == 1) {
      plot(seq(from = f, to = max(intervals), by = s),
      1 / m,
      type = "l",
      col = n + 1, ylab="req/sec", xlab="time (minutes)", main=title)
    } else {
      lines(seq(from = f, to = max(intervals), by = s),
      1 / m,
      type = "l",
      col = n + 1)
    }
  }
}