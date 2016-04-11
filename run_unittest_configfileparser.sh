#!/bin/bash

# determine if we're running on a Mac (specify specific version of
# gcc/g++ to avoid , or in a Travis CI VM
# (Travis CI defines TRAVIS=true)
if [[ $OSTYPE == darwin* ]]
then
  CPP=g++
  CC=gcc
#   CPP=g++-5
#   CC=gcc-5
else
  CPP=g++
  CC=gcc
fi

# Set the path to cxxtestgen depending on whether we're running under Travis or not
if env | grep -q ^TRAVIS=
then
  CXXTEST=/usr
else
  CXXTEST=/usr/local/cxxtest-4.4
fi
CXXTESTGEN=$CXXTEST/bin/cxxtestgen

echo
echo "Generating and compiling unit tests for config-file parser..."
$CXXTESTGEN --error-printer -o test_runner_config.cpp unit_tests/unittest_config_parser.t.h
$CPP -o test_runner_config test_runner_config.cpp core/config_file_parser.cpp core/utilities.cpp -I. -Icore -Ic_code -Isolvers -I/usr/local/include -I$CXXTEST
if [ $? -eq 0 ]
then
  echo "Running unit tests for config_file_parser:"
  ./test_runner_config
  exit
else
  echo "Compilation of unit tests for config_file_parser.cpp failed."
  exit 1
fi