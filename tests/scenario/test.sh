#!/bin/sh

. ../test-lib.sh

"$COMPASS" -s -f test.scn 2>&1 >output.txt

if [ $? -ne 0 ]; then
	exit 1
fi

fix-summary summary.dat summary-out.dat

diff summary-target.dat summary-out.dat 2>&1 >/dev/null

