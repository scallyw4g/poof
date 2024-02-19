#!/usr/bin/env bash

# NOTE(Jesse): These should be removed eventually.. but I'm leaving them here
# for the time-being since it should be harmless
#
# NOTE(Jesse): The following are switches for twiddling during development.
# Calling functions by name on the command line shouldn't be affected by these.

# RunPreemptivePoof=1

# BuildAllBinariesRunAllTests=1

# FetchBonsaiDebug=0

# BuildPoof=1
# RunPoof=1

# POOF_LOG_LEVEL="--log-level LogLevel_Debug"
# POOF_DEBUGGER="gdb --args"

# BuildPoofEmcc=0
# RollupEmcc=0

# BuildParserTests=1
# RunParserTests=1
# TEST_LOG_LEVEL="--log-level LogLevel_Debug"
# TEST_DEBUGGER="gdb --args"

# BuildAndRunAllExamples=1

# RunIntegrationTests=1
# INTEGRATION_TEST_INDEX=1
# INTEGRATION_TEST_LOG_LEVEL="--log-level LogLevel_Debug -c0"
# INTEGRATION_TEST_DEBUGGER="gdb --args"

# RunExtendedIntegrationTests=1

# OPTIMIZATION_LEVEL="-O2"
# BONSAI_INTERNAL="-D BONSAI_INTERNAL=0"




stdlib_build_scripts='include/bonsai_stdlib/scripts'
. $stdlib_build_scripts/preamble.sh
. $stdlib_build_scripts/setup_for_cxx.sh
. include/bonsai_debug/make.sh

if [ $# -gt 1 ]; then
  echo "make.sh supports a maximum of 1 argument"
fi

# NOTE(Jesse): Can't do this cause it fucks with the paths poof outputs
# ROOT="$(pwd)"
ROOT="."
SRC="$ROOT/poof"
BIN="$ROOT/bin"
BIN_TEST="$BIN/tests"
INCLUDE="include"
META_OUT="generated"
EXTENDED_INTEGRATION_TESTS_SRC="$ROOT/tests/integration_extended"


function RollupEmcc
{
  if [ -x docs/node_modules/.bin/rollup ]; then

   docs/node_modules/.bin/rollup                               \
     docs/main.js                                              \
      -f iife                                                  \
      -o docs/main.bundle.js                                    \
      -p @rollup/plugin-node-resolve
  else
    echo -e "$Error Rollup not found, try running './make.sh BootstrapWeb'"
  fi
}

function BuildPoofEmcc
{
  ColorizeTitle "Building Poof (emcc)"

  which emcc > /dev/null
  if [ $? -eq 0 ]; then
    emcc                                     \
      poof/poof.cpp                          \
      $OPTIMIZATION_LEVEL                    \
      $CXX_OPTIONS                           \
      $PLATFORM_CXX_OPTIONS                  \
      $PLATFORM_LINKER_OPTIONS               \
                                             \
      -O0                                    \
      -g                                     \
                                             \
      -msimd128                              \
      -sALLOW_MEMORY_GROWTH                  \
      -sEXPORTED_FUNCTIONS=_DoPoofForWeb     \
      -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
      -D "__SSE__"                           \
      -D "BONSAI_EMCC"                       \
      -Wno-disabled-macro-expansion          \
      -Wno-reserved-identifier               \
      -Wno-limited-postlink-optimizations    \
                                             \
      -sFORCE_FILESYSTEM                     \
      --preload-file examples                \
                                             \
      -sASSERTIONS=2                         \
      -sSAFE_HEAP=1                          \
      -sSTACK_OVERFLOW_CHECK=1               \
      -fsanitize=undefined                   \
                                             \
      $BONSAI_INTERNAL                       \
      $PLATFORM_INCLUDE_DIRS                 \
      -I "$ROOT"                             \
      -I "$ROOT/include"                     \
      -I "$ROOT/poof"                        \
      -o docs/poof_runtime.js


    if [ $? -eq 0 ]; then
      echo -e "$Success poof/poof.cpp -> poof.html"
    else
      echo -e "$Failed Error building poof, exiting."
      exit 1
    fi

  if [ -d docs/node_modules ]; then
    RollupEmcc
  else
    echo -e "$Info ./docs/node_modules could not be found! Attempting to bootstrap"
    BootstrapWeb
    if [ -d docs/node_modules ]; then
      RollupEmcc
    fi
  fi

  else
    echo -e "$Warn No installation of emcc detected"
    echo -e "$Info If you wish to target WASM, install emscriptend then run './make.sh BootstrapWeb'"
  fi

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""
}

function BootstrapWeb
{
  pushd docs > /dev/null 2>&1

  which npm > /dev/null
  if [ $? -eq 0 ]; then
    npm install > /dev/null 2>&1
    if [ $? -eq 0 ]; then
      echo -e "$Success BootstrapWeb complete!"
    else
      echo -e "$Failed Bootstrapping docs"
    fi
  else
    echo -e "$Error No installation of npm detected.  To bootstrap docs, please install npm"
  fi
  popd > /dev/null 2>&1
}


function RunPoof {
  ColorizeTitle "Poofing!"

  if [ -d $META_OUT ]; then
    rm -Rf $META_OUT
  fi

  : "${POOF_EXECUTABLE:=./bin/poof}"

    # --do-debug-window \

  # POOF_LOG_LEVEL="--log-level LogLevel_Debug"
  $POOF_DEBUGGER $POOF_EXECUTABLE \
                                  \
    $POOF_LOG_LEVEL               \
    poof/poof.cpp                 \
    -D POOF_PREPROCESSOR          \
    -D BONSAI_PREPROCESSOR        \
    $PLATFORM_DEFINES             \
    -I "."                        \
    -I "$ROOT/include"            \
    -o $META_OUT

  if [ $? -eq 0 ]; then
    echo -e "$Success Poofed"
  else
    echo -e "$Failed Poofing"
    git checkout $META_OUT
  fi

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""

}

function RunInstalledPoof {
  old_poof_executable=$POOF_EXECUTABLE
  POOF_EXECUTABLE=poof
  old_poof_debugger=$POOF_DEBUGGER
  POOF_DEBUGGER=
  old_poof_log_level=$POOF_LOG_LEVEL
  # POOF_LOG_LEVEL="--log-level LogLevel_Error"

  RunPoof

  POOF_LOG_LEVEL=$old_poof_log_level
  POOF_DEBUGGER=$old_poof_debugger
  POOF_EXECUTABLE=$old_poof_executable
}

function BuildPoof {

  which clang++ > /dev/null
  [ $? -ne 0 ] && echo -e "Please install clang++" && exit 1

  ColorizeTitle "Building Poof"
  source_file="$SRC/poof.cpp"
  SetOutputBinaryPathBasename "$source_file" bin
  echo -e "$Building $source_file"

  clang++                                 \
    $source_file                          \
    $OPTIMIZATION_LEVEL                   \
    $CXX_OPTIONS                          \
    $PLATFORM_CXX_OPTIONS                 \
    $PLATFORM_LINKER_OPTIONS              \
    $PLATFORM_DEFINES                     \
    $BONSAI_INTERNAL                      \
    $PLATFORM_INCLUDE_DIRS                \
    -I "$ROOT"                            \
    -I "$ROOT/include"                    \
    -I "$ROOT/poof"                       \
    -o "$output_basename""$PLATFORM_EXE_EXTENSION"

  if [ $? -eq 0 ]; then
    echo -e "$Success $source_file -> $full_output_name"
    echo -e ""
    echo -e "$Delimeter"
    echo -e ""

  else
   echo ""
   echo -e "$Failed Error building poof, exiting."
   exit 1
  fi
}

function RunIntegrationTests()
{
  ColorizeTitle "Running Integration Tests"

  INTEGRATION_SRC_DIR=tests/integration/src
  INTEGRATION_OUTPUT_DIR=tests/integration/generated

  FILES_TO_POOF=$(find $INTEGRATION_SRC_DIR -type f | grep '\.h')

  test_index=0
  for filename in $FILES_TO_POOF; do

    basename_stripped=$(basename $filename .h)

    test_output_dir=$INTEGRATION_OUTPUT_DIR/$basename_stripped

    # Each test file has a directory created under the output directory that
    # we'll write all the poof() output to for that test file.  We diff that
    # whole directory to confirm the output is identical for that test file

    run_test=1
    if [[ -v INTEGRATION_TEST_INDEX ]]; then
      if [[ $test_index != $INTEGRATION_TEST_INDEX ]]; then
        run_test=0
      fi
    fi


    if [[ $run_test == 1 ]]; then

      if [ -d $test_output_dir ]; then
        rm -Rf $test_output_dir
      fi

      mkdir -p $test_output_dir

      echo -e "$Info TestIndex($test_index)"
      echo -e "$Indent $INTEGRATION_TEST_DEBUGGER bin/poof $POOF_COLOR_FLAG $INTEGRATION_TEST_LOG_LEVEL $filename -o $test_output_dir"

      $INTEGRATION_TEST_DEBUGGER bin/poof \
        $POOF_COLOR_FLAG \
        $INTEGRATION_TEST_LOG_LEVEL       \
        $filename                         \
        -o $test_output_dir

      if [[ $? == 0 ]]; then
        DIFF_CMD="git diff --ignore-all-space --ignore-blank-lines ${test_output_dir} | wc -l"

        echo -e "$Indent $DIFF_CMD == ($DIFF_CHANGED)"

        DIFF_CHANGED=$(eval $DIFF_CMD)
        if [[ $DIFF_CHANGED == 0 ]]; then
          echo -e "$Success $filename"
        else
          echo -e "$Failed $filename"
        fi
      else
        echo -e "$Failed $filename"
      fi
    fi

    echo ""

    ((test_index++))
  done

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""
}

function BuildParserTests
{
  ColorizeTitle "Building Tests"
    source_file=tests/parser/poof.cpp
    SetOutputBinaryPathBasename "$source_file" bin/tests
    echo -e "$Building $source_file"

    clang++                                 \
      $source_file                          \
      $OPTIMIZATION_LEVEL                   \
      $CXX_OPTIONS                          \
      $PLATFORM_CXX_OPTIONS                 \
      $PLATFORM_LINKER_OPTIONS              \
      $PLATFORM_DEFINES                     \
      $PLATFORM_INCLUDE_DIRS                \
      $BONSAI_INTERNAL                      \
      -I "."                                \
      -I "$ROOT/include"                    \
      -I "$ROOT/poof"                       \
    -o "$output_basename""$PLATFORM_EXE_EXTENSION"

  if [ $? -eq 0 ]; then
    echo -e "$Success $source_file -> $full_output_name"
    echo -e ""
    echo -e "$Delimeter"
    echo -e ""

  else
   echo ""
   echo -e "$Failed Error building $source_file, exiting."
   echo ""
   exit 1
  fi

}

function RunParserTests
{
  ColorizeTitle "Running Parser Tests"
  $TEST_DEBUGGER ./bin/tests/poof \
    $TEST_LOG_LEVEL               \

  echo -e "$Delimeter"
  echo -e ""
}

function RunSingleExtendedIntegrationTest
{
  if [ -d $EXTENDED_INTEGRATION_TESTS_SRC/$test_name ]; then
    pushd "$EXTENDED_INTEGRATION_TESTS_SRC/$test_name" > /dev/null

    $1

    if [ $? -eq 0 ]; then
      echo -e "$Success $test_name parsed"
    else
     echo -e "$Failed $test_name didn't parse"
    fi

    popd > /dev/null
  else
    echo -e "$Warn $test_name not found.  Try running './make.sh BootstrapExtendedIntegrationTests'"
  fi
}

function RunExtendedIntegrationTests
{
  ColorizeTitle "Running Extended Integration Tests"

  test_name=uacme
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
      -D USE_OPENSSL               \
      ./uacme.c"

  test_name=redis
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
    --log-level LogLevel_Shush     \
    -D BYTE_ORDER                  \
    -D LITTLE_ENDIAN               \
                                   \
    -D __clang__                   \
    -D __i386                      \
    -D __x86_64__                  \
    -D linux                       \
    -D __linux__                   \
    -D __GNUC__                    \
     src/server.c"

  echo "           - NOTE(Jesse): poof is known to not handle redis correctly yet.  This failure is expected"

  test_name=sqlite
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
    -D __clang__                   \
    -D __i386                      \
    -D __x86_64__                  \
    -D linux                       \
    -D __linux__                   \
    -D __GNUC__                    \
     src/test_server.c"

  test_name=handmade_hero
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
      code/handmade.cpp"

  test_name=bonsai
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
      -I src/                      \
      -I src/poof                  \
      -I include/                  \
      -D BONSAI_LINUX              \
      -o src/generated             \
      src/game_loader.cpp"

  test_name=hcc
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
      -I src/                      \
      src/hcc.c"

  test_name=gf
  RunSingleExtendedIntegrationTest \
    "$BIN/poof                     \
      -I .                         \
      gf2.cpp"


  echo -e ""
  echo -e "$Delimeter"
  echo -e ""
}

function BootstrapSingleExtendedIntegrationTest
{
  if [ -d $EXTENDED_INTEGRATION_TESTS_SRC/$test_name ]; then
    echo -e "$Info $test_name exists, skipping"
  else
    echo -e "$Info Cloning $test_name"
    git clone $1 $test_repo $EXTENDED_INTEGRATION_TESTS_SRC/$test_name
  fi
}

function BootstrapExtendedIntegrationTests
{
  ColorizeTitle "Bootstrapping Extended Integration Test Suite"

  test_name=uacme
  test_repo=git@github.com:ndilieto/uacme
  BootstrapSingleExtendedIntegrationTest

  test_name=redis
  test_repo=git@github.com:$test_name/$test_name
  BootstrapSingleExtendedIntegrationTest

  test_name=sqlite
  test_repo=git@github.com:$test_name/$test_name
  BootstrapSingleExtendedIntegrationTest

  test_name=handmade_hero
  test_repo=git@github.com:HandmadeHero/cpp
  BootstrapSingleExtendedIntegrationTest

  test_name=bonsai
  test_repo=git@github.com:scallyw4g/bonsai
  BootstrapSingleExtendedIntegrationTest '--recursive'

  test_name=hcc
  test_repo=https://github.com/heroseh/hcc
  BootstrapSingleExtendedIntegrationTest

  test_name=gf
  test_repo=https://github.com/nakst/gf
  BootstrapSingleExtendedIntegrationTest
}


function BuildAndRunAllExamples
{
  ColorizeTitle "Building and Running examples"
  pushd examples > /dev/null
  ./make_all_examples.sh > /dev/null

  if [ $? -eq 0 ]; then
    echo -e "$Success Examples built and ran"
  else
   echo -e "$Failed Examples didn't build, or hit runtime errors"
  fi

  popd > /dev/null

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""
}

function FetchBonsaiDebug
{
  [ -d lib_bonsai_debug ] && rm -Rf lib_bonsai_debug
  cp -R ../bonsai/releases/debug_lib lib_bonsai_debug
}

function BuildAllBinaries
{
  BuildPoof
  BuildPoofEmcc
  BuildParserTests
}

function RunAllTests
{
  RunParserTests
  RunIntegrationTests
  RunExtendedIntegrationTests
}

function BuildAllBinariesRunAllTests
{
  BuildAllBinaries
  RunAllTests
  BuildAndRunAllExamples
}


if [ ! -d "$BIN" ]; then
  mkdir "$BIN"
fi

if [ ! -d "$BIN_TEST" ]; then
  mkdir "$BIN_TEST"
fi

echo -e ""
echo -e "$Delimeter"
echo -e ""





: "${OPTIMIZATION_LEVEL:="-O0"}"
: "${BONSAI_INTERNAL:="-D BONSAI_INTERNAL=1"}"

# If someone supplied a command line argument, call the function, otherwise
# respect the 'runflags'
if [ $# -eq 1 ]; then

  # If someone's just trying to run the build/tests from the CLI normally don't
  # throw a bunch of garbage output
  TEST_LOG_LEVEL="--log-level LogLevel_Error"
  POOF_LOG_LEVEL="--log-level LogLevel_Error"

  # If someone's just trying to run the build/tests from the CLI normally don't
  # throw them into a debugging environment
  POOF_DEBUGGER=""
  TEST_DEBUGGER=""

  "$@"

else

  BuildAllBinariesRunAllTests

fi
