#!/bin/sh

if [ $# -lt 1 ]; then
	echo "usage: $0 config [run-number(s)|all [ini-file]]"
	echo example:
	echo "  "$0 Reactive 0..2
	exit 1
fi


{ ../../examples/simple_am/simple_am localhost; }&
AMPID=$!; 

trap 'kill $AMPID;' INT

./run.sh $*

wait $AMPID

echo "done!"


