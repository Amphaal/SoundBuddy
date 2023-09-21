#!/bin/bash

if [[ ! -d $1 ]]; then
    echo "Expecting executable folder containing executable and libraries that might miss dependencies, [$1] is not valid"
    exit 1
fi

# activate globstar
shopt -s globstar

# safe create destination 
mkdir -p ${CMAKE_BINARY_DIR}/PEDeps_${component}

# list all files to scan
files_to_scan=$(ls -p $1 | grep -v / | tr '\n' ' ')

cd $1

# find dependencies && ignore any stderr output into a variable
deps=$(${PELDD_EXEC} --ignore-errors -t $files_to_scan 2> /dev/null)

# copy dependencies
echo "$deps" | tr -d '\r' | xargs -i cp {} ${CMAKE_BINARY_DIR}/PEDeps_${component}

# disable globstar
shopt -u globstar