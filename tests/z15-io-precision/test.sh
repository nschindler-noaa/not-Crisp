#!/bin/sh

. ../test-lib.sh

"$COMPASS" -s -f test1.scn -o test1-tmp.scn 2>&1 >output.txt

if [ $? -ne 0 ]; then
	exit 1
fi

fix-scenario test1-tmp.scn test1-out.scn

diff test1-target.scn test1-out.scn 2>&1 >/dev/null

