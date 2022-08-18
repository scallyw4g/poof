#! /usr/bin/env bash

[ ! -d tmp ] && mkdir tmp || :

FILES_TO_POOF=$(find integration -type f | grep '\.h')

for filename in $FILES_TO_POOF; do

  basename_stripped=$(basename $filename .h)

  ../bin/poof_dev \
    $filename \
    -o generated/$basename_stripped

  DIFF_CHANGED=$(git diff --ignore-all-space --ignore-blank-lines generated/$basename_stripped | wc -l)

  if [[ $DIFF_CHANGED == 0 ]]; then
    echo "PASS $filename"
  else
    echo "FAIL $filename"
  fi



done
