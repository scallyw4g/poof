#! /usr/bin/env bash

. scripts/preamble.sh
. scripts/setup_for_cxx.sh

# ./make.sh BuildPoof
OPTIMIZATION_LEVEL="-O2" BONSAI_INTERNAL=0 ./make.sh BuildPoof
cp bin/poof$PLATFORM_EXE_EXTENSION ~/bin/poof

