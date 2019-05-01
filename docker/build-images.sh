#!/bin/bash
CURDIR=`pwd`
DIR=`dirname $0`
cd $DIR/omnetpp-vnc
docker build . -t martinpfannemueller/omnetpp-vnc:5.4.1
cd ..
docker build . -t martinpfannemueller/swim:1.0
cd $CURDIR

