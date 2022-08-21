#! /usr/bin/env bash

# TODO(Jesse): How can we root scripts at the project root?
cd ~/work/poof

. scripts/preamble.sh
. scripts/setup_for_cxx.sh

function RunIntegrationTests()
{
  INTEGRATION_SRC_DIR=tests/integration/src
  INTEGRATION_OUTPUT_DIR=tests/integration/generated

  FILES_TO_POOF=$(find $INTEGRATION_SRC_DIR -type f | grep '\.h')

  for filename in $FILES_TO_POOF; do

    basename_stripped=$(basename $filename .h)

    # Each test file has a directory created under the output directory that
    # we'll write all the meta() output to for that test file.  We diff that
    # whole directory to confirm/deny the output is identical for that test file

    # gdb --args bin/poof_dev \
    bin/poof_dev              \
      $filename               \
      $COLORFLAG              \
      -o $INTEGRATION_OUTPUT_DIR/$basename_stripped

    DIFF_CHANGED=$(git diff --ignore-all-space --ignore-blank-lines $INTEGRATION_OUTPUT_DIR/$basename_stripped | wc -l)

    if [[ $DIFF_CHANGED == 0 ]]; then
      echo "PASS $filename"
    else
      echo "FAIL $filename"
    fi
  done
}

function BuildParserTests
{
  echo ""
  ColorizeTitle "Tests"
    executable=tests/parser/preprocessor.cpp
    SetOutputBinaryPathBasename "$executable" bin/tests
    echo -e "$Building $executable"
    clang++                                          \
      $OPTIMIZATION_LEVEL                            \
      $CXX_OPTIONS                                   \
      $PLATFORM_CXX_OPTIONS                          \
      $PLATFORM_LINKER_OPTIONS                       \
      $PLATFORM_DEFINES                              \
      $PLATFORM_INCLUDE_DIRS                         \
      -I"."                                          \
      -I"include"                                    \
      -o "$output_basename""$PLATFORM_EXE_EXTENSION" \
      $executable && echo -e "$Success $executable -> $output_basename$PLATFORM_EXE_EXTENSION"
}

function RunParserTests
{
  gdb --args ./bin/tests/preprocessor $COLORFLAG
  # ./bin/tests/preprocessor $COLORFLAG
}



#######################################
$@
#######################################

