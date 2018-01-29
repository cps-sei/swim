# Simulator of Web Infrastructure and Management
This guide briefly explains how to plot simulation results with R.

## Installing R and required packages
Install R and some extra packages
```
sudo apt-get install r-base r-cran-reshape r-cran-ggplot2 r-cran-rsqlite
R -e 'install.packages("https://github.com/wilkelab/cowplot/archive/0.6.3.tar.gz", repos=NULL)'
```

## Plot simulation results
Now, you can run R and plot the simulation results. Start R with the command `R`, and then execute these commands (directory path may change depending on how SWIM was installed):
```
setwd('~/seams-swim/results')
source('../swim/tools/plotResults.R')
plotResults("sim", folder="SWIM", run=1)
```
If you want to save the plot as PDF or PNG, pass a fourth parameter `saveAs` with the path to the file. For example:
```
plotResults("sim", folder="SWIM", run=1, saveAs="results.pdf")
plotResults("sim", folder="SWIM", run=1, saveAs="results.png")
```

If you want the plot to include a chart for the instantaneous utility too use the argument `instantaneousUtility=TRUE`.

If you want the plot to have a minor grid for each period, use the argument `periodGrid=TRUE`.

Note that when executing the `plotResults()` function, it prints out metrics of the run, such as the total accumulated utility. This information is in a suitable format for a table in LaTeX.

By default, the utility is computed using the utility function in the paper [Comparing model-based predictive approaches to self-adaptation: CobRA and PLA](https://works.bepress.com/gabriel_moreno/33/). Other utility functions can be defined and passed as the argument `utilityFc`. Keep in mind that these utility functions must operate on vectors.

Also, there are good environments for R, such as [RStudio](https://www.rstudio.com/)
