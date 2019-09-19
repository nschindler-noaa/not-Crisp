#!/bin/sh

if [ -z "$1" ]; then
	echo "USAGE: $0 COMPASS-BINARY [TEST-NUMBER] [RIVER-DESCRIPTION]"
	echo "TEST-NUMBER can be the number of a specific test to be run or - for all tests (this is the default)"
	exit 1
fi

if [ -x "$PWD/$1" ]; then
	export COMPASS="$PWD/$1"
elif [ -x "$1" ]; then
	export COMPASS="$1"
else
	echo "Could not find COMPASS binary (or it's not executable)"
	exit 1
fi

if [ -z "$BASH" ]; then
	BASH='/bin/bash'
fi

if [ -z "$2" ]; then
	run='-'
else
	if [ "$2" != '-' ]; then
		if [ "$2" -gt 0 ]; then
			run="$2"
		else
			run='-'
		fi
	else
		run="$2"
	fi
fi

if [ -z "$3" ]; then
	export COLUMBIA="../columbia.desc"
else
	export COLUMBIA="$3"
fi

if [ ! -x "$COLUMBIA" ]; then
	echo "Could not locate river description file"
	exit 1
fi

echo "========== Running COMPASS Unit Tests =========="
echo "$COMPASS"
echo "================================================"
echo ""

count=0
failures=0
passes=0
skips=0
for i in *; do
	if [ -d "$i" ]; then
		if [ -x "$i/test.sh" ]; then
			count=`expr $count + 1`
			if [ "$run" != '-' ];then if [ "$run" -ne "$count" ]; then
				skips=`expr $skips + 1`
				continue
			fi; fi
			printf "Running ($count)	$i...	"

			cp "$COLUMBIA" "$i"
			cd "$i"
			rm -f *logfile*
			"$BASH" test.sh
			result="$?"
			cd ..


			if [ "$result" -ne 0 ]; then
				echo "FAILED"
				echo
				echo "*** FAILED $i ***"
				if [ -f "$i/purpose.txt" ]; then
					cat "$i/purpose.txt"
				fi
				echo
				failures=`expr $failures + 1`
			else
				echo "PASSED"
				passes=`expr $passes + 1`
			fi
		fi
	fi
done

echo
correct=`expr $skips + $passes`
tested=`expr $count - $skips`
if [ $passes -eq $count ]; then
	printf "PASSED all $count tests"
elif [ $correct -eq $count ]; then
	printf "PASSED"
elif [ $skips -gt 0 ]; then
	printf "FAILED"
else
	printf "FAILED $failures out of $tested tests"
fi
if [ $skips -gt 0 ]; then
	echo " (skipped the remaining $skips tests)"
else
	echo
fi

exit $failures

