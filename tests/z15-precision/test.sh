#!/bin/sh

. ../test-lib.sh

"$COMPASS" -s -f test1.scn 2>&1 >output.txt

if [ $? -ne 0 ]; then
	exit 1
fi

fix-summary summary.dat summary1.dat
grep -y passed summary1.dat > summary1-passed.dat

"$COMPASS" -s -f test2.scn 2>&1 >output.txt

if [ $? -ne 0 ]; then
	exit 1
fi

fix-summary summary.dat summary2.dat
grep -y passed summary2.dat > summary2-passed.dat

diff summary1-passed.dat summary2-passed.dat 2>&1 >/dev/null
result="$?"

if [ $result -ne 0 ]; then
	exit 0
else
	exit 1
fi

