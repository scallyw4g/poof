#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit

[ -d generated ] && rm -Rf generated
mkdir generated

  # --log-level LogLevel_Error \

../../bin/poof_dev           \
  -I "."                     \
  --output-path ./generated  \
  main.c

clang main.c      \
  -o main         \
  -I "."


echo "_ _ _ _ example program output begin _ _ _ _"
echo ""
./main
echo ""
echo "_ _ _ _ example program output end _ _ _ _"
