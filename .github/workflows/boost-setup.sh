pushd /tmp/
  wget --quiet https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.tar.gz
  tar -xf boost_1_73_0.tar.gz
  pushd boost_1_73_0
    ./bootstrap.sh > /dev/null
    sudo ./b2 link=shared runtime-link=shared threading=multi install > /dev/null
  popd
popd
