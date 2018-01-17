#!/bin/sh
{ ../../../simple_am/simple_am localhost; }&
AMPID=$!; 

trap 'kill $AMPID;' INT

./run.sh $*

wait $AMPID

echo "done!"


