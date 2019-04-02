#!/bin/bash
# Запускать только из корня проекта
prjnamed=$(basename $PWD)
prjname=$(echo $prjnamed | rev | cut -c 2- | rev)

refname=$(git rev-parse --abbrev-ref HEAD)
buildtype=$1
shared_static=$2
prefix=$3
shared="ON"

if [[ "$(python3 ./.ci/wamba-ci.py has deploy.$buildtype )" == "OFF" ]]; then
  echo "Do not deploy for build type $buildtype"
  exit 0
fi

if [[ "$shared_static" == "static" ]]; then
  if [[ "$(python3 ./.ci/wamba-ci.py option deploy.$buildtype.static )" == "OFF" ]]; then
    echo "Do not deploy for $buildtype-static"
    exit 0
  fi
  shared="OFF"
elif [[ "$shared_static" == "shared" ]]; then
  if [[ "$(python3 ./.ci/wamba-ci.py option deploy.$buildtype.shared )" == "OFF" ]]; then
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
  echo cmake .. -DCMAKE_BUILD_TYPE=$buildtype -DBUILD_SHARED_LIBS=$shared -DCMAKE_CXX_STANDARD=$standard "${@:4}" || exit $?
  cmake .. -DCMAKE_BUILD_TYPE=$buildtype -DBUILD_SHARED_LIBS=$shared \
           -DCMAKE_CXX_STANDARD=$standard -DCMAKE_CXX_COMPILER=$compiler "${@:4}" || exit $?
  cmake --build . -- -j$build_threads || exit $?
popd

function deploy()
{
  if [[ "$(python3 ./.ci/wamba-ci.py option deploy.$buildtype.$1 )" == "OFF" ]]; then
    echo "Do not deploy for configurations $shared_static-$buildtype-$1"
    return
  fi

  rm -rf ./build/deploy/lead-up/*
  cp -rf ./build/$buildtype/bin/* ./build/deploy/lead-up/
  cp ./configurations/* ./build/deploy/lead-up/ 2>/dev/null || true
  cp -rfH ./configurations/common/* ./build/deploy/lead-up/ 2>/dev/null || true
  cp -rfH ./configurations/$1/* ./build/deploy/lead-up/ 2>/dev/null || true
  cp ./.ci/start.sh "./build/deploy/lead-up/${prjname}.sh" 2>/dev/null || true
  arname="$prefix$prjnamed-$refname-$compiler$standard-$shared_static-$buildtype-$1.tar.gz"
  escaped_arname=$(printf '%s\n' "$arname" | sed 's:[\/&]:\\&:g;$!s/$/\\/')
  cat ./.ci/deploy.sh | sed -e "s/replaceme/${escaped_arname}/g" > ./build/deploy/lead-up/deploy.sh
  chmod +x ./build/deploy/lead-up/deploy.sh
  chmod +x ./build/deploy/lead-up/${prjname}.sh
  
  arpath="./build/deploy/$arname"
  tar -zcvf "$arpath" -C "./build/deploy/lead-up/" .
  mkdir -p /monamour/$prjname
  cp -f "$arpath" /monamour/$prjname
}

if [ ! -z "$(ls -A ./build/$buildtype/bin/)" ]; then
  mkdir -p ./build/deploy
  mkdir -p ./build/deploy/lead-up/
  deploy production
  deploy devel
fi
