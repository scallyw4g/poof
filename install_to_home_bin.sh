#! /usr/bin/env bash

# ./make.sh BuildPoof
OPTIMIZATION_LEVEL="-O2" BONSAI_INTERNAL=0 ./make.sh BuildPoof
cp bin/poof ~/bin/poof

