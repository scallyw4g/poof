#!/usr/bin/env bash

# export POOF_LOG_LEVEL=LogLevel_Debug

./make.sh RunInstalledPoof
./make.sh BuildAllBinariesRunAllTests

# FetchBonsaiDebug

# ./make.sh RunInstalledPoof
# ./make.sh BuildPoof
# ./make.sh RunPoof

# export OPTIMIZATION_LEVEL="-O2"

# ./make.sh RunInstalledPoof
# OPTIMIZATION_LEVEL="-O2" \
# ./make.sh BuildPoof
# [ $? -ne 0 ] && echo "Build Failed, exiting." && exit 1

# INTEGRATION_TEST_INDEX=4 \
# INTEGRATION_TEST_LOG_LEVEL="--log-level LogLevel_Debug" \
# ./make.sh RunIntegrationTests

# ./make.sh BuildDebugSystem

# ./make.sh RunIntegrationTests
# ./make.sh BuildAndRunAllExamples

# RollupEmcc
# BuildPoofEmcc

# ./make.sh BuildParserTests
# ./make.sh RunParserTests




# RunExtendedIntegrationTests

