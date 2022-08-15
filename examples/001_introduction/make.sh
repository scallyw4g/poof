#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit

clang main.c -o introduction
./introduction
echo "Our example program compiled and returned $?, hooray!"
