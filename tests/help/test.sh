#!/bin/sh

usage="$("$COMPASS" --help |grep -y usage)"
if [ -z "$usage" ]; then
	exit 1
else
	exit 0
fi

