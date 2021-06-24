#!/bin/bash

sudo locale-gen ru_RU
sudo locale-gen ru_RU.UTF-8
sudo update-locale
sudo apt-get install -y lcov doxygen
if [[ "${GITHUB_REPOSITORY}" == *"prefixdb"* ]] || [[ "${GITHUB_REPOSITORY}" == *"btp"* ]]; then
  sudo apt-get install libsnappy-dev
  sudo apt-get install zlib1g-dev
  sudo apt-get install libbz2-dev
  sudo apt-get install liblz4-dev
  sudo apt-get install libzstd-dev
fi
