#!/bin/bash
compiler=$1
standard=$2
buildtype=$3

if [[ "$(python3 ./.ci/wamba-ci.py enabled build-and-test.$buildtype)" != "1" ]]; then
  echo "Do not $buildtype build (disabled)"
  exit 0
fi

if [[ "$(python3 ./.ci/wamba-ci.py enabled build-and-test.$compiler.$standard)" != "1" ]]; then
  echo "Do not $buildtype build for $compiler.$standard (disabled)"
  exit 0
fi

build_threads="$(python3 ./.ci/wamba-ci.py get build_threads 4)"

date
mkdir -p build
cd build || exit $?
rm -rf *
cmake -DCMAKE_CXX_COMPILER=$compiler -DCMAKE_CXX_STANDARD=$standard -DCMAKE_BUILD_TYPE=$buildtype -DBUILD_TESTING=ON -DPARANOID_WARNING=ON "${@:3}" .. || exit $?
cmake --build . -- -j$build_threads || exit $?
ctest --output-on-failure || exit $?
make clean
