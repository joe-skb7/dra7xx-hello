#!/bin/sh

# $1 - param name
# $2 - file name
# $3 - CONFIG_BOOT

val=$(cat $2 | grep -v '^\s*//' | grep -v '^\s/\*' | grep $1 \
	| awk '{print $3}' | tr '\n' ' ' | awk "{print \$$3}")
if [ -z "$val" ]; then
	echo " ** Error: Param \"$1\" not found" >&2
	echo -n "    val = " >&2
	echo $val >&2
	exit 1
fi

echo $val
