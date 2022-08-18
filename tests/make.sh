#! /usr/bin/env bash

[ ! -d tmp ] && mkdir tmp || :

FILES_TO_POOF=$(find integration -type f | grep '\.h')

for filename in $FILES_TO_POOF; do

  basename_stripped=$(basename $filename .h)

  ../bin/poof_dev \
    $filename \
    -o generated/$basename_stripped

  git diff generated/$basename_stripped

done
