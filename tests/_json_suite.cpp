#include <fas/testing.hpp>
#include <wjson/_json.hpp>


UNIT(_json1, "")
{
  using namespace fas::testing;
  t << nothing;
  #if __cplusplus >= 201103L
    using namespace wjson::literals;
    t << equal<assert, std::string>("'test'"_json, "\"test\"");
  #endif
}

BEGIN_SUITE(_json, "")
  ADD_UNIT(_json1)
END_SUITE(_json)
