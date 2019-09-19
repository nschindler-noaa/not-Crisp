#!/bin/sh

. ../test-lib.sh

"$COMPASS" -s -f test.scn 2>&1 >output1.txt
if [ $? -ne 0 ]; then
	exit 1
fi
fix-summary summary.dat summary-out1.dat
"$COMPASS" -s -f test.scn 2>&1 >output2.txt
if [ $? -ne 0 ]; then
	exit 1
fi
fix-summary summary.dat summary-out2.dat

diff summary-out1.dat summary-out2.dat |grep -y survival 2>&1 >/dev/null

