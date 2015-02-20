#!/bin/sh

# Replace named constants in linker script using C preprocessor
# $1    - original linker script file
# $2    - file name for generated linker script
# $3... - CFLAGS

in=$1
out=$2
shift
shift
cflags=$*

echo "/* This file was generated automatically."	>$out
echo " * Please do not modify it manually!"		>>$out
echo " */"						>>$out
echo							>>$out
gcc -E -x c -Iinclude $cflags $in | grep -v '^#'	>>$out
