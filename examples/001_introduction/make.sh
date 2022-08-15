#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit

clang ./main.c -o main

./main

echo "Our example program compiled and returned $?, hooray!"
