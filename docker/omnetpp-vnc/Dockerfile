FROM consol/ubuntu-xfce-vnc:1.2.3
LABEL Description="Docker image for OMNeT++ Network Simulator with VNC"

USER root
RUN apt update && apt install -y --no-install-recommends build-essential gcc g++ bison flex perl \
	qt5-default tcl-dev tk-dev libxml2-dev zlib1g-dev default-jre \
	doxygen graphviz libwebkitgtk-3.0-0 wget xvfb libqt5opengl5-dev

RUN wget --header="Accept: text/html" \
         --referer="https://omnetpp.org" \
         --output-document=omnetpp-5.2.1-src.tgz \
         https://omnetpp.org/omnetpp/send/30-omnet-releases/2321-omnetpp-5-2-1-linux

RUN tar --directory=/opt -xzf omnetpp-5.2.1-src.tgz && rm omnetpp-5.2.1-src.tgz

WORKDIR /opt/omnetpp-5.2.1

ENV PATH $PATH:/opt/omnetpp-5.2.1/bin

# Configure and compile omnet++
RUN xvfb-run ./configure WITH_OSG=no WITH_OSGEARTH=no && make

# Cleanup
RUN apt-get clean && \
  rm -rf /var/lib/apt && \
  rm -f /omnetpp-5.2.1-src.tgz

WORKDIR $HOME

