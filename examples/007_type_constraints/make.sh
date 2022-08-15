#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit


# rm generated/*

# gdb --args          \
../../bin/poof_dev  \
  -I "."            \
  -o generated      \
  main.c

clang main.c      \
  -o example      \
  -I "."

./example
