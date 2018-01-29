This is an example of an adaptation manager as an external program using the TCP-based interface of SWIM.

The documentation of the TCP interface can be found in [docs/ExternalControl.pdf](../../docs/ExternalControl.pdf).

The class `SwimClient` encapsulates the TCP connection with SWIM, and can be reused to implement other adaptation managers.

To use this adaptation manager, launch first SWIM and then run `simple_am`. The script [simulations/swim/run-sa.sh](../../simulations/swim/run-sa.sh) automates this.

