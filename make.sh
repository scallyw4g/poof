#! /bin/bash

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

. scripts/preamble.sh
. scripts/setup_for_cxx.sh

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

  #  --log-level LogLevel_Error \
  # gdb --args bin/poof_dev                 \
  bin/poof_dev                 \
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


if [ ! -d "$BIN" ]; then
  mkdir "$BIN"
fi

if [ ! -d "$BIN_TEST" ]; then
  mkdir "$BIN_TEST"
fi

if [[ $BuildPoof == 1 || $BUILD_EVERYTHING == 1 ]]; then
  BuildPoof
fi

if [[ $RunPoof == 1 || $BUILD_EVERYTHING == 1 ]]; then
  RunPoof
fi

if [[ $BuildParserTests == 1 || $BUILD_EVERYTHING == 1 ]]; then
  ./tests/make.sh BuildParserTests
fi


if [[ $RunParserTests == 1 || $BUILD_EVERYTHING == 1 ]]; then
  ./tests/make.sh RunParserTests
fi

if [[ $RunIntegrationTests == 1 ]]; then
  ./tests/make.sh RunIntegrationTests
fi
