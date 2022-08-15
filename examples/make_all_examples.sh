#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit


for example_code in ./**/main.c; do
  echo "Building $example_code"
  pushd "$(dirname $example_code)" > /dev/null 2>&1
  ../make_individual_example.sh
  popd > /dev/null 2>&1
  echo ""
  echo ""
done
