# SWIM: Simulator of Web Infrastructure and Management

## Installation
This covers how to install it in Ubuntu.

The first step is to install OMNeT++, the simulation framework.

- Download [OMNeT++ 5.4.1](https://omnetpp.org/download/)
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
To run the simulation and look at the results, follow the instructions in the main [README.md](README.md#running-swim)
