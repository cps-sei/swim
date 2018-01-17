#!/bin/bash
if [ $# -ne 4 ]; then
	echo usage: $0 resultsFolder configuration run plotFilename\{.pdf\|.png\}
	exit 1
fi

SRCDIR=`dirname $0`

R -q -e "source('$SRCDIR/plotResults.R'); plotResults('$2', '$1', run=$3, saveAs='$4')"

gnome-open $4
