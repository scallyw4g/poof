#! /bin/bash

. scripts/preamble.sh
. scripts/setup_for_cxx.sh

if [ $# -gt 1 ]; then
  echo "make.sh supports a maximum of 1 argument"
fi

ROOT="."
SRC="$ROOT/poof"
BIN="$ROOT/bin"
BIN_TEST="$BIN/tests"
META_OUT="$ROOT/poof/generated"


BUILD_EVERYTHING=0

# git checkout $META_OUT

BuildPoof=1
RunPoof=1

BuildParserTests=1
RunParserTests=1

RunIntegrationTests=1

OPTIMIZATION_LEVEL="-O0"

function RunPoof {

  # NOTE(Jesse): This currently doesn't work (well, it could be forced to)
  # because poof/generated/generate_cursor_c_token.h isn't generated and I don't
  # really want to move it out of there since a med-term goal is to be able to
  # augment generated structures with additional members
  #
  # If you really want, add:
  #
  # git checkout poof/generated/generate_cursor_c_token.h
  #
  # to special-case that one file.
  #

  # if [ -d $META_OUT ]; then
  #   rm -Rf $META_OUT
  #   mkdir -p $META_OUT
  #   mkdir -p $META_OUT/tmp
  #   git checkout poof/generated/generate_cursor_c_token.h
  # fi

  # gdb --args bin/poof_dev                 \
  bin/poof_dev                 \
    --log-level LogLevel_Error \
    poof/preprocessor.cpp      \
    $COLORFLAG                 \
    -D BONSAI_PREPROCESSOR     \
    -D BONSAI_LINUX            \
    -I "."                     \
    -I "include"               \
    -o $META_OUT
}

function BuildPoof {
  which clang++ > /dev/null
  [ $? -ne 0 ] && echo -e "Please install clang++" && exit 1

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""

  ColorizeTitle "Building Preprocessor"
  executable="$SRC/preprocessor.cpp"
  echo -e "$Building $executable"
  clang++                                                \
    $OPTIMIZATION_LEVEL                                  \
    $CXX_OPTIONS                                         \
    $PLATFORM_CXX_OPTIONS                                \
    $PLATFORM_LINKER_OPTIONS                             \
    $PLATFORM_DEFINES                                    \
    -D "BONSAI_DEBUG_SYSTEM_API"                         \
    $PLATFORM_INCLUDE_DIRS                               \
    -I "$ROOT"                                           \
    -I "$ROOT/include"                                   \
    -o "bin/poof_dev""$PLATFORM_EXE_EXTENSION"           \
    $executable

  if [ $? -eq 0 ]; then
   echo -e "$Success $executable"

    echo -e ""
    echo -e "$Delimeter"
    echo -e ""

  else
   echo ""
   echo -e "$Failed Error building preprocessor, exiting."
   exit 1
  fi
}

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

    # gdb --args bin/poof_dev    \
    bin/poof_dev                 \
      $filename                  \
      $COLORFLAG                 \
      --log-level LogLevel_Error \
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
  # gdb --args ./bin/tests/preprocessor $COLORFLAG
  ./bin/tests/preprocessor     \
    --log-level LogLevel_Shush \
    $COLORFLAG
}


function BuildAll
{
  BuildPoof
  BuildParserTests
}

function RunAllTests
{
  RunParserTests
  RunIntegrationTests
}

function BuildAndRunAll
{
  BuildAll
  RunAll
}


if [ ! -d "$BIN" ]; then
  mkdir "$BIN"
fi

if [ ! -d "$BIN_TEST" ]; then
  mkdir "$BIN_TEST"
fi

# If someone supplied a command line argument, call the function, otherwise
# respect the runflags
if [ $# -eq 1 ]; then

  "$@"

else

  if [[ $BuildPoof == 1 || $BUILD_EVERYTHING == 1 ]]; then
    BuildPoof
  fi

  if [[ $RunPoof == 1 || $BUILD_EVERYTHING == 1 ]]; then
    RunPoof
  fi

  if [[ $BuildParserTests == 1 || $BUILD_EVERYTHING == 1 ]]; then
    BuildParserTests
  fi


  if [[ $RunParserTests == 1 || $BUILD_EVERYTHING == 1 ]]; then
    RunParserTests
  fi

  if [[ $RunIntegrationTests == 1 ]]; then
    RunIntegrationTests
  fi

fi
