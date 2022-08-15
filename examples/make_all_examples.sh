#!/usr/bin/env bash

set -o nounset
set -o pipefail
set -o errexit


for build_script in ./**/make.sh; do
  echo "Executing $build_script"
  pushd "$(dirname $build_script)" > /dev/null 2>&1
  ./make.sh
  popd > /dev/null 2>&1
  echo ""
  echo ""
done
