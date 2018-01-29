# SWIM: Simulator of Web Infrastructure and Management

## Installation
This covers how to install it in Ubuntu.

The first step is to install OMNeT++, the simulation framework.

- Download [OMNeT++ 5.2](https://omnetpp.org/omnetpp)
- Install OMNeT++ following the [installation guide](https://omnetpp.org/doc/omnetpp/InstallGuide.pdf)


Install required libraries:
```
sudo apt-get install libboost-all-dev
```

This simulation requires a modification to the queueing library that comes with OMNeT++, and a socket scheduler that also comes with OMNet++. Both are provided in `queueinglib.tgz`. Follow one of the following procedures depending on whether the installation is done from archives or from a git repository.

### Installation from downloaded archive files
Both `queueinglib.tgz` and `swim.tgz` must be extracted in the same directory. For example

``` 
cd ~
mkdir swim
cd swim
tar xzf ~/Downloads/swim.tgz
tar xzf ~/Downloads/queueinglib.tgz 
```

### Installation from git repository
Both `queueinglib` and `swim` must be in the same directory. For example

``` 
cd ~
mkdir swim
cd swim
git clone https://github.com/cps-sei/swim.git
tar xzf swim/queueinglib.tgz 
```

## Compiling
Next the library must be compiled
```
cd queueinglib
make
cd ..
```

Now, the simulation can be compiled
```
cd swim
make cleanall && make makefiles && make
cd ..
```

## Running the simulation
Before running the simulation, check the entry `result-dir` in `simulations/swim/swim.ini`. The path is the directory where the result files while be saved. Note that this path is also used in `runexp.sh`, so if it is changed, it must be changed in both files.

To run the simulation:
```
cd swim/simulations/swim
./runexp.sh sim 1
```

where `sim` is a configuration defined in `simulations/swim/swim.ini`, and `1` is a run number (see documentation for parameter studies in the OMNeT++ manual). Optionally, a third argument with the path of an `.ini` file can be passed.

To look at the results, one option is to launch OMNeT++, and create a project changing its location to the value of `result-dir` in the `.ini` file.
The results are stored in the files `sim-*.sca` and `sim-*.vec`. The former contains scalar values corresponding to the configuration of the experiment and results recorded at the end of the run, such as the accumulated utility. The `.vec` file contains vectors of different metrics collected during the run, such as the utility accrued per period, the number of servers, and active servers, the arrival rate, etc.
The data in these files can be explored and plotted using the Analysis Editor in OMNeT++ (see the chapter 9 in [OMNeT++ User Guide](https://omnetpp.org/doc/omnetpp/UserGuide.pdf)) or any other tool that can read SQLite databases.  