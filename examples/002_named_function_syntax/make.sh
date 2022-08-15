#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit

for filename in ./generated/*; do rm $filename; done

../../bin/poof_dev  \
  -I "."            \
  -o generated      \
  main.c

clang main.c      \
  -o main         \
  -I "."

./main

echo "Our example program compiled and returned $?, hooray!"
