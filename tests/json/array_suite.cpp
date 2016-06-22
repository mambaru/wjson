#include <fas/testing.hpp>
#include <iow/json/json.hpp>


UNIT(number_array, "")
{
  using namespace fas::testing;
  t << nothing;
}


BEGIN_SUITE(array, "")
  ADD_UNIT(number_array)
END_SUITE(array)