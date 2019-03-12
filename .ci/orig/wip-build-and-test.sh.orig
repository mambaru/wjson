#!/bin/bash

build_threads="$(python3 ./.ci/wamba-ci.py get build_threads 4)"

date
mkdir -p build
cd build || exit $?
rm -rf *
cmake -DBUILD_TESTING=ON "${@}" .. || exit $?
cmake --build . -- -j$build_threads || exit $?
ctest --output-on-failure || exit $?
