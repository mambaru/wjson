#!/bin/bash
# Запускать только из корня проекта
prjname=$(basename $PWD)
refname=$(git rev-parse --abbrev-ref HEAD)
buildtype=$1
shared_static=$2
shared="ON"

if [[ "$(python3 ./.ci/wamba-ci.py has deploy.$buildtype )" != "1" ]]; then
  echo "Do not deploy for build type $buildtype"
  exit 0
fi

if [[ "$shared_static" == "static" ]]; then
  if [[ "$(python3 ./.ci/wamba-ci.py enabled deploy.$buildtype.static )" != "1" ]]; then
    echo "Do not deploy for $buildtype-static"
    exit 0
  fi
  shared="OFF"
elif [[ "$shared_static" == "shared" ]]; then
  if [[ "$(python3 ./.ci/wamba-ci.py enabled deploy.$buildtype.shared )" != "1" ]]; then
    echo "Do not deploy for $buildtype-shared"
    exit 0
  fi
else
  echo "shared or static"
  exit 1
fi

standard="$(python3 ./.ci/wamba-ci.py get deploy.standard 11)"
compiler="$(python3 ./.ci/wamba-ci.py get deploy.compiler g++)"
build_threads="$(python3 ./.ci/wamba-ci.py get build_threads 4)"

mkdir -p build
pushd build
  #rm -rf *
  echo cmake .. -DCMAKE_BUILD_TYPE=$buildtype -DBUILD_SHARED_LIBS=$shared -DCMAKE_CXX_STANDARD=$standard "${@:3}" || exit $?
  cmake .. -DCMAKE_BUILD_TYPE=$buildtype -DBUILD_SHARED_LIBS=$shared \
           -DCMAKE_CXX_STANDARD=$standard -DCMAKE_CXX_COMPILER=$compiler "${@:3}" || exit $?
  cmake --build . -- -j$build_threads || exit $?
popd

function deploy()
{
  if [[ "$(python3 ./.ci/wamba-ci.py enabled deploy.$buildtype.$1 )" != "1" ]]; then
    echo "Do not deploy for configurations $shared_static-$buildtype-$1"
    return
  fi

  rm -rf ./build/deploy/lead-up/*
  cp -rf ./build/$buildtype/bin/* ./build/deploy/lead-up/
  cp -rfH ./configurations/$prjname/common/* ./build/deploy/lead-up/ 2>/dev/null || true
  cp -rfH ./configurations/$prjname/$1/* ./build/deploy/lead-up/ 2>/dev/null || true
  arname="./build/deploy/$prjname-$refname-$compiler$standard-$shared_static-$buildtype-$1.tar.gz"
  tar -zcvf "$arname" -C "./build/deploy/lead-up/" .
  mkdir -p /monamour/$prjname
  cp -f "$arname" /monamour/$prjname
}

if [ ! -z "$(ls -A ./build/$buildtype/bin/)" ]; then
  mkdir -p ./build/deploy
  mkdir -p ./build/deploy/lead-up/
  deploy production
  deploy loads
  deploy stress
  deploy devel
fi
