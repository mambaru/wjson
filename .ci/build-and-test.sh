#!/bin/bash
date
mkdir -p build
cd build || exit $?
rm -rf *
cmake -DCMAKE_CXX_COMPILER=$1 -DCMAKE_CXX_STANDARD=$2 -DCMAKE_BUILD_TYPE=$3 -DBUILD_TESTING=ON -DPARANOID_WARNING=ON "${@:3}" .. || exit $?
cmake --build . -- -j8 || exit $?
ctest --output-on-failure
make clean
