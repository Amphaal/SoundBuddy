#!/bin/bash

# activate globstar
shopt -s globstar

# safe create destination 
mkdir -p ${CMAKE_BINARY_DIR}/PEDeps_${component}

# find all targets libraries
targets=$(ls ${pattern_})

# find dependencies
deps=$(${PELDD_EXEC} -t --ignore-errors $targets)

# copy dependencies
echo "$deps" | tr -d '\r' | xargs -i cp {} ${CMAKE_BINARY_DIR}/PEDeps_${component}

# disable globstar
shopt -u globstar