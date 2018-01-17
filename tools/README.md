# Simulator of Web Infrastructure and Management
This guide briefly explains how to plot simulation results with R.

## Installing R and OMNeT++ R plugin
Install R and some extra packages needed by the plugin
```
sudo apt-get install r-base r-cran-plyr r-cran-reshape2
```

To install OMNeT++ R plugin, download it from [link](https://omnetpp.org/models/tools-utilities)

The installation instructions are [here](https://github.com/omnetpp/omnetpp-resultfiles/wiki/Tutorial-for-the-omnetpp-r-package), but basically the following does it. First, start R from a terminal (`R` is the name of the executable).
Once you get the R prompt, execute the following command:
```
install.packages('omnetpp')
```
When asked if you want to use and create a personal library answer yes and yes.
You will get an error saying the package is not available. That's fine. Now, quit R by entering the command `quit()`, if you're asked if you want to save a workspace image, the answer is not relevant at this point.
Now, install the plugin executing this in a terminal:
```
R CMD INSTALL ~/Downloads/omnetpp_0.6-1.tar.gz
```
## Install plot scripts and additional package
If the `plotResults.R` file was not included in the SWIM distribution, create a directory `misc/swimEval` in the same directory in which `swim` and `queueinglib` are, and place the `plotResults.R` file there.

There is an additional R package needed to generate the plots. Start R with the command `R`, and then execute this command:
```
install.packages("cowplot")
```

## Plot simulation results
Now, you can run R and plot the simulation results. Start R with the command `R`, and then execute these commands:
```
setwd('~/swim/misc/swimEval')
source('plotResults.R')
plotResults2("sim", folder="SWIM", run=1)
```
Note that the path to the directory in the `setwd()` command must be adjusted if SWIM has not been installed in the place suggested by the instructions. If you want to save the plot as PDF or PNG, pass a fourth parameter `saveAs` with the path to the file. For example:
```
plotResults2("sim", folder="SWIM", run=1, saveAs="results.pdf")
plotResults2("sim", folder="SWIM", run=1, saveAs="results.png")
```

If you want the plot to include a chart for the instantaneous utility too use the argument `instantaneousUtility=TRUE`.

If you want the plot to have a minor grid for each period, use the argument `periodGrid=TRUE`.

Note that when executing the `plotResults2()` function, it prints out metrics of the run, such as the total accumulated utility. This information is in a suitable format for a table in LaTeX.

By default, the utility is computed using the utility function in the paper [Efficient Decision-Making under Uncertainty for Proactive Self-Adaptation](https://works.bepress.com/gabriel_moreno/28/). Other utility functions can be defined and passed as the argument `utilityFc`. Keep in mind that these utility functions must operate on vectors.

Also, there are good environments for R, such as [RStudio](https://www.rstudio.com/)
