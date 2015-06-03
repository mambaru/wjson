#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>
#include <fas/testing.hpp>
#include <algorithm>


UNIT(outgoing1_unit, "")
{
  using namespace fas::testing;
  using namespace ::iow::jsonrpc;
  
}




BEGIN_SUITE(outgoing_suite, "")
  ADD_UNIT(outgoing1_unit)
END_SUITE(outgoing_suite)