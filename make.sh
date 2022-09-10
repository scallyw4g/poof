#! /bin/bash

# set -o nounset
# set -o pipefail
# set -o errexit

# NOTE(Jesse): The following are switches for twiddling during development.
# Calling functions by name on the command line shouldn't be affected by these.


BuildAllBinariesRunAllTests=1

# RunPreemptivePoof=1

# RunPoof=1
# BuildPoof=1
# POOF_LOG_LEVEL="--log-level LogLevel_Debug"
# POOF_DEBUGGER="gdb --args"

# BuildEmcc=1

# RunParserTests=1
# BuildParserTests=1
# TEST_LOG_LEVEL="--log-level LogLevel_Debug"
# TEST_DEBUGGER="gdb --args"

# BuildAndRunAllExamples=1

# RunIntegrationTests=1
# INTEGRATION_TEST_INDEX=1
# INTEGRATION_TEST_LOG_LEVEL="--log-level LogLevel_Debug"
# INTEGRATION_TEST_DEBUGGER="gdb --args"

# RunExtendedIntegrationTests=1

# OPTIMIZATION_LEVEL="-O2"





. scripts/preamble.sh
. scripts/setup_for_cxx.sh

if [ $# -gt 1 ]; then
  echo "make.sh supports a maximum of 1 argument"
fi

ROOT="$(pwd)"
SRC="$ROOT/poof"
BIN="$ROOT/bin"
BIN_TEST="$BIN/tests"
META_OUT="poof/generated"
EXTENDED_INTEGRATION_TESTS_SRC="$ROOT/tests/integration_extended"

function BuildEmcc
{
  which emcc > /dev/null
  [ $? -ne 0 ] && echo -e "Please install emcc" && exit 1

  ColorizeTitle "Building Poof (emcc)"

  emcc                            \
    poof/poof.cpp                 \
    $OPTIMIZATION_LEVEL           \
    $CXX_OPTIONS                  \
    $PLATFORM_CXX_OPTIONS         \
    $PLATFORM_LINKER_OPTIONS      \
                                  \
    -msimd128                     \
    -sSINGLE_FILE                 \
    -sALLOW_MEMORY_GROWTH         \
    -D "__SSE__"                  \
    -D "BONSAI_EMCC"              \
    -Wno-disabled-macro-expansion \
    -Wno-reserved-identifier      \
                                  \
    -D "BONSAI_INTERNAL"          \
    $PLATFORM_INCLUDE_DIRS        \
    -I "$ROOT"                    \
    -I "$ROOT/include"            \
    -I "$ROOT/poof"               \
    -o poof.html

  if [ $? -eq 0 ]; then
    echo -e "$Success poof/poof.cpp -> poof.html"
    echo -e ""
    echo -e "$Delimeter"
    echo -e ""

  else
   echo ""
   echo -e "$Failed Error building poof, exiting."
   exit 1
  fi

}

# git checkout $META_OUT


function RunPoof {
  ColorizeTitle "Poofing!"

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

  if [ -d $META_OUT ]; then
    rm -Rf $META_OUT
    git checkout poof/generated/generate_cursor_c_token.h

    git checkout poof/generated/for_all_datatypes_debug_print_functions.h
    git checkout poof/generated/for_all_datatypes_debug_print_prototypes.h
    git checkout poof/generated/debug_print_ast_node.h
    git checkout poof/generated/debug_print_datatype.h
    git checkout poof/generated/debug_print_declaration.h
    git checkout poof/generated/debug_print_prototype_datatype.h
    git checkout poof/generated/debug_print_prototype_declaration.h

  fi

  : "${POOF_EXECUTABLE:=./bin/poof}"

  $POOF_DEBUGGER $POOF_EXECUTABLE \
                                  \
    $POOF_LOG_LEVEL               \
    poof/poof.cpp                 \
    -D BONSAI_PREPROCESSOR        \
    -D BONSAI_LINUX               \
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

function BuildPoof {

  which clang++ > /dev/null
  [ $? -ne 0 ] && echo -e "Please install clang++" && exit 1

  ColorizeTitle "Building Poof"
  source_file="$SRC/poof.cpp"
  SetFullOutputName "$source_file" bin
  echo -e "$Building $source_file"

  clang++                        \
    $source_file                 \
    $OPTIMIZATION_LEVEL          \
    $CXX_OPTIONS                 \
    $PLATFORM_CXX_OPTIONS        \
    $PLATFORM_LINKER_OPTIONS     \
    $PLATFORM_DEFINES            \
    -D "BONSAI_DEBUG_SYSTEM_API" \
    $PLATFORM_INCLUDE_DIRS       \
    -I "$ROOT"                   \
    -I "$ROOT/include"           \
    -I "$ROOT/poof"           \
    -o "$full_output_name"

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

    if [ ! -d $test_output_dir ]; then
      mkdir $test_output_dir
    fi

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
      $INTEGRATION_TEST_DEBUGGER bin/poof \
        $INTEGRATION_TEST_LOG_LEVEL       \
        $filename                         \
        -o $test_output_dir

      if [[ $? == 0 ]]; then
        DIFF_CHANGED=$(git diff --ignore-all-space --ignore-blank-lines $test_output_dir | wc -l)
        if [[ $DIFF_CHANGED == 0 ]]; then
          echo -e "$Success $filename"
        else
          echo -e "$Failed $filename"
        fi
      else
        echo -e "$Failed $filename"
      fi
    fi

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
    SetFullOutputName "$source_file" bin/tests
    echo -e "$Building $source_file"

    clang++                    \
      $source_file             \
      $OPTIMIZATION_LEVEL      \
      $CXX_OPTIONS             \
      $PLATFORM_CXX_OPTIONS    \
      $PLATFORM_LINKER_OPTIONS \
      $PLATFORM_DEFINES        \
      $PLATFORM_INCLUDE_DIRS   \
      -I "."                   \
      -I "$ROOT/include"       \
      -I "$ROOT/poof"          \
      -o "$full_output_name"

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
  test_repo=git@github.com:jjbandit/bonsai
  BootstrapSingleExtendedIntegrationTest '--recursive'
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

function BuildAllBinaries
{
  BuildPoof
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

  if [[ $RunPreemptivePoof == 1 ]]; then
    old_poof_executable=$POOF_EXECUTABLE
    POOF_EXECUTABLE=poof
    old_poof_debugger=$POOF_DEBUGGER
    POOF_DEBUGGER=
    old_poof_log_level=$POOF_LOG_LEVEL
    POOF_LOG_LEVEL="--log-level LogLevel_Error"

    RunPoof

    POOF_LOG_LEVEL=$old_poof_log_level
    POOF_DEBUGGER=$old_poof_debugger
    POOF_EXECUTABLE=$old_poof_executable
  fi

  if [[ $BuildPoof == 1 ]]; then
    BuildPoof
  fi

  if [[ $BuildEmcc == 1 ]]; then
    BuildEmcc
  fi

  if [[ $RunPoof == 1 ]]; then
    RunPoof
  fi

  if [[ $BuildParserTests == 1 ]]; then
    BuildParserTests
  fi

  if [[ $BuildAndRunAllExamples == 1 ]]; then
    BuildAndRunAllExamples
  fi



  if [[ $RunParserTests == 1 ]]; then
    RunParserTests
  fi

  if [[ $RunIntegrationTests == 1 ]]; then
    RunIntegrationTests
  fi

  if [[ $RunExtendedIntegrationTests == 1 ]]; then
    RunExtendedIntegrationTests
  fi

  if [[ $BuildAllBinariesRunAllTests == 1 ]]; then
    BuildAllBinariesRunAllTests
  fi

fi
