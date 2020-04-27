# SWIM - A Simulator of Web Infrastructure and Management
SWIM is a self-adaptive exemplar that simulates a web application. SWIM can be used as a target system with an external adaptation manager interacting with it through its TCP-based interface or with an adaptation manager built as a simulation module.

## Installation - Install Docker
SWIM uses Docker containers to automate deployment of the simulation into a virtual container. Docker is available for free at http://docker.com. Docker must be installed in your environment. The community edition is sufficient for running SWIM, which can be downloaded from [here](https://www.docker.com/community-edition#/download). SWIM itself is hosted on Docker Hub.

## Creating and connecting to a container
As mentioned, SWIM is hosted on Docker Hub, and online repository for Docker containers. The first time docker is run, containers will be downloaded onto your host machine (multiple downloads will occur because containers are layered on each other, and SWIM is built on a number of Linux containers for windowing, simulation, etc.). This can be done with the following command run in a terminal or command prompt:
```
   > docker run -d -p 5901:5901 -p 6901:6901 --name swim gabrielmoreno/swim
```

The output will be something like this as it is downloading the Docker images.
```
   Unable to find image 'gabrielmoreno/swim:1.0' locally
   1.0: Pulling from gabrielmoreno/swim
   50aff78429b1: Pulling fs layer 
   f6d82e297bce: Pulling fs layer 
   275abb2c8a6f: Pulling fs layer 
   9f15a39356d6: Pulling fs layer 
   fc0342a94c89: Waiting 
   5a83c621ae24: Waiting 
   7bb74dfa8c81: Waiting 
   8a9116b32ff1: Pulling fs layer 
   edf226a814de: Waiting 
   8f9a373766f9: Waiting 
   f18e2de6417a: Pull complete 
   0b651a0d9a51: Pull complete 
   ...
   Digest: sha256:059cbcb47e4da4e1c1b081cb1634cd15fbf4b8789ef5af9f89b57b5c97f57182
   Status: Downloaded newer image for gabrielmoreno/swim:1.0
```

When all images have `Pull complete` and the final message is printed, the SWIM container is now installed on your system.

**Note:** Don't do this now, but when you are done using the container, you can stop the container using ``docker stop swim``, and to start it again you can do ``docker start swim``.

### Connecting to SWIM
The container opens two ports on your host (5901 and 6901) that are mapped to the same ports in the container. These ports allow two modes of connection with SWIM - via HTTP or via a VNC client.

If you have a VNC client installed, you can connect through it to the host: `vnc://localhost:5901`, and using the password `vncpassword`. To connect with HTTP, open a HTML5 compatible browser, and open the address http://localhost:6901 This will also prompt you for the password (which is the same as above). 

## Running SWIM
SWIM can be accessed in several modes:

1. With an adaptation as a simulation module
2. With the included external adaptation manager
3. With another external adaptation manager that uses the TCP interface of SWIM

### How to run simulation with adaptation manager as a module:
1. Start a Terminal Emulator from the `Applications` menu in the top-left corner. This opens a terminal within the container. 
2. In this terminal, type the commands 
```
   > cd ~/seams-swim/swim/simulations/swim_sa/
   > ./run.sh Reactive 1
```
The arguments for the run command are as follows:
```
./run.sh config [run-number(s)|all [ini-file]]}
```
The config argument is a named configuration defined in the ini file.
The optional argument to select which run to execute can include a list of comma-separated run numbers without spaces (e.g., `1,3,4`), or a range or run numbers (e.g., `0-4`).
The optional third argument can be used to specify a different ini file (the default is `swim_sa.ini`).
For details about how configurations and runs are defined see the [OMNeT++ manual](https://omnetpp.org/doc/omnetpp/manual). With the default ini file, run 1 uses the WorldCup trace with a server boot delay of 60 seconds.  

When the previous command is run, you will see the output of the events in the simulation.
```
   ../../src//swim swim_sa.ini -u Cmdenv -c Reactive -n ..:../../src:....
   Reactive run 1: $trace=...
   ...
   All runs completed
```
3. After the simulation has finished, the results for this are put in the directory	 `~/seams-swim/results/SWIM_SA` which defines a SQLite database of all the results. These results can be plotted using R, whose use is automated through a shell script. Open another terminal emulator, and type the commands below, after which a plot of the simulation will be displayed (and saved in the file `plot.pdf`).
Alternatively, you can use a file name with a `.png` extension to generate a PNG file.
```
   > cd ~/seams-swim/results/
   > ../swim/tools/plotResults.sh SWIM_SA Reactive 1 plot.pdf
```

#### Comparing with another adaptation manager
SWIM includes a second sample adaptation manager to show how changes in adaptation managers can be compared.
The adaptation manager used by the configuration `Reactive` removes a server if the response time is below the threshold and the dimmer setting is at the maximum, _and if there is more than one server of spare capacity_.
A configuration named `Reactive2` uses a second adaptation manager that is the same as the previous one, except that this one does not have the condition regarding the spare capacity.

To see how these two adaptation managers compare, run the experiment and generate a new plot with the following commands.
```
   > cd ~/seams-swim/swim/simulations/swim_sa/
   > ./run.sh Reactive2 1
   > cd ~/seams-swim/results/
   > ../swim/tools/plotResults.sh SWIM_SA Reactive2 1 plot2.pdf
```
If the previous plot was closed, it can be reopened with `gnome-open plot.pdf`.
Comparing the two results we can see that the second adaptation manager misses the response time requirement more often than the first one and obtains less utility.

### How to run simulation with included external adaptation manager:
This option uses the simple adaptation manager found in `swim/examples/simple_am`. This will run in real time, and **will take 1 hour 45 minutes (the duration of the trace) to run.**

1. Start a Terminal Emulator from the Applications menu in the top-left corner, and type the commands:
```
   > cd ~/seams-swim/swim/simulations/swim/
   > ./run-sa.sh sim 1
```
2. To display the results, open a new terminal emulator and type the commands:
```
   > cd ~/seams-swim/results/
   ../swim/tools/plotResults.sh SWIM sim 1 plot.png
```

### How to run simulation with another external adaptation manager:
The same as above, except that the script `run.sh` has to be used instead. This script launches SWIM but not the external adaptation manager, which has to be launched independently.

## Source code and OMNeT++ IDE
The Docker container includes the OMNeT++ IDE with the source code for SWIM and the example adaptation managers included.

To launch the OMNeT++ IDE, launch Applications > Development > OMNeT++.

The external adaptation manager is included in the project `simple_am` in the work space.
The adaptation manager that works as a simulation module, which is called `ReactiveAdaptationManager`, is located in `swim/src/managers/adaptation/`.
