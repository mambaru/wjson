#include <fas/testing.hpp>
#include <algorithm>


UNIT(value1_unit, "")
{
  using namespace fas::testing;
  t << nothing;
}




BEGIN_SUITE(value_suite, "")
  ADD_UNIT(value1_unit)
END_SUITE(value_suite)