#!/bin/sh

# Run tests (passed in as arguments) and append the results
# onto results.txt using this (tab-separated) format:
#
# seconds in epoch, tab, number passed, tab, number failed

passed=0
failed=0

# Run each test and record the result
for i in $@; do
	printf '\t'
	./$i > /dev/null
	ret=$?
	if [ $ret -eq 0 ]; then
		passed=`expr $passed + 1`
	else
		failed=`expr $failed + 1`
		printf "*** Failure in $i\n\n"
	fi
done

# Append results onto results.txt
echo "`date +%s`	$passed	$failed" >> results.txt

# If there were any failures return failure
if [ $failed -gt 0 ]; then
	exit -1
fi

