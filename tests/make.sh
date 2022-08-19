#! /usr/bin/env bash

# TODO(Jesse): How can we root scripts at the project root?
cd ~/work/poof

. scripts/preamble.sh
. scripts/setup_for_cxx.sh

FILES_TO_POOF=$(find tests/integration -type f | grep '\.h')

for filename in $FILES_TO_POOF; do

  basename_stripped=$(basename $filename .h)

  # gdb --args bin/poof_dev \
  bin/poof_dev \
    $filename             \
    $COLORFLAG            \
    -o tests/generated/$basename_stripped

  DIFF_CHANGED=$(git diff --ignore-all-space --ignore-blank-lines tests/generated/$basename_stripped | wc -l)

  if [[ $DIFF_CHANGED == 0 ]]; then
    echo "PASS $filename"
  else
    echo "FAIL $filename"
  fi



done
