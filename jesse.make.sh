#!/usr/bin/env bash

# ./make.sh BuildAllBinariesRunAllTests

# BuildAllBinariesRunAllTests

# FetchBonsaiDebug

# ./make.sh RunInstalledPoof
# ./make.sh BuildPoof
# ./make.sh RunPoof

# ./make.sh RunInstalledPoof
./make.sh BuildPoof

# INTEGRATION_TEST_INDEX=2 \
# INTEGRATION_TEST_LOG_LEVEL="--log-level LogLevel_Debug" \
# ./make.sh RunIntegrationTests

./make.sh RunIntegrationTests
# ./make.sh BuildAndRunAllExamples

# RollupEmcc
# BuildPoofEmcc

# ./make.sh BuildParserTests
# ./make.sh RunParserTests




# RunExtendedIntegrationTests

