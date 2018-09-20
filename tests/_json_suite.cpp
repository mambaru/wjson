#include <fas/testing.hpp>
#include <wjson/json.hpp>
#include <wjson/error.hpp>
#include <wjson/strerror.hpp>


UNIT(_json1, "")
{
  using namespace fas::testing;
  using namespace wjson;
  t << fail("test");
}

BEGIN_SUITE(_json, "")
  ADD_UNIT(_json1)
END_SUITE(_json)
