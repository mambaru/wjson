#include <iostream>
#include <iow/acceptor/acceptor_base.hpp>
#include <fas/testing.hpp>

UNIT(acceptor, "")
{
  using namespace fas::testing;
  t << nothing;
}

BEGIN_SUITE(acceptor,"acceptor")
  ADD_UNIT(acceptor)
END_SUITE(acceptor)

BEGIN_TEST
  RUN_SUITE(acceptor)
END_TEST
