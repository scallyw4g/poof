#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit


../../bin/poof_dev  \
  -I "."            \
  -o generated      \
  main.c

clang main.c      \
  -o introduction \
  -I "."
./introduction
echo "Our example program compiled and returned $?, hooray!"
