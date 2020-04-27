#!/bin/bash
CURDIR=`pwd`
DIR=`dirname $0`
cd $DIR/omnetpp-vnc
docker build . -t gabrielmoreno/omnetpp-vnc:5.4.1
cd ..
docker build . -t gabrielmoreno/swim:1.0.1
cd $CURDIR

