#! /usr/bin/env bash

stdlib_build_scripts='include/bonsai_stdlib/scripts'
. $stdlib_build_scripts/preamble.sh
. $stdlib_build_scripts/setup_for_cxx.sh

# ./make.sh BuildPoof
OPTIMIZATION_LEVEL="-O2" BONSAI_INTERNAL=1 ./make.sh BuildPoof
cp bin/poof$PLATFORM_EXE_EXTENSION ~/bin/poof

