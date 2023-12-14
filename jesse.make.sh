#!/usr/bin/env bash

. make.sh


BuildAllBinariesRunAllTests

# FetchBonsaiDebug

RunInstalledPoof
BuildPoof
RunPoof

# RollupEmcc
# BuildPoofEmcc

# BuildParserTests
# RunParserTests


# RunIntegrationTests

# BuildAndRunAllExamples

# RunExtendedIntegrationTests

