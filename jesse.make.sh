#!/usr/bin/env bash

# ./make.sh BuildAllBinariesRunAllTests


# BuildAllBinariesRunAllTests

# FetchBonsaiDebug

./make.sh RunInstalledPoof
./make.sh BuildPoof
# ./make.sh RunPoof

# ./make.sh RunInstalledPoof
# ./make.sh BuildPoof
./make.sh RunIntegrationTests
./make.sh BuildAndRunAllExamples

# RollupEmcc
# BuildPoofEmcc

# BuildParserTests
# RunParserTests




# RunExtendedIntegrationTests

