#! /bin/bash

BUILD_EVERYTHING=1
BuildPoof=1

. scripts/preamble.sh
. scripts/setup_for_cxx.sh

OPTIMIZATION_LEVEL="-O0"

ROOT="."
SRC="$ROOT/poof"
BIN="$ROOT/bin"
BIN_TEST="$BIN/tests"
META_OUT="$SRC/poof/generated"

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
