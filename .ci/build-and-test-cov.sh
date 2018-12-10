#!/bin/bash
date
mkdir -p build
cd build || exit $?
rm -rf *
cmake -DCMAKE_CXX_COMPILER=$1 -DCMAKE_CXX_STANDARD=$2 -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DBUILD_COVERAGE=ON "${@:3}" .. || exit $?
cmake --build . -- -j8 || exit $?
ctest --output-on-failure
coveralls -r tests/CMakeFiles/test_json.dir/ --gcov-options '\-lpr'
