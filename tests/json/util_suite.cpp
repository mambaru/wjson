#include <fas/testing.hpp>
#include <iow/json/json.hpp>
#include <algorithm>

UNIT(util1, "raw_value")
{
  using namespace fas::testing;
  using namespace iow::json;

  std::string json="\"Привет мир!\"";
  std::string value;
  raw_value< std::string >::serializer ser;
  ser( value, json.begin(), json.end(), 0 );
  t << equal<expect>(json, value) << FAS_TESTING_FILE_LINE;
  json.clear();;
  ser( value, std::back_inserter(json) );
  t << equal<expect>(json, value) << FAS_TESTING_FILE_LINE;
}

UNIT(util2, "raw_range")
{
  using namespace fas::testing;
  using namespace iow::json;

  std::string json="\"Привет мир!\"";
  typedef std::pair<std::string::iterator, std::string::iterator> range_t;
  range_t value;
  raw_pair<range_t>::serializer ser;
  ser( value, json.begin(), json.end(), 0 );
  //value.first++;
  t << equal<expect>(json, std::string(value.first, value.second)) << FAS_TESTING_FILE_LINE;
  json.clear();
  //value.first++;
  ser( value, std::back_inserter(json) );
  t << equal<expect>(json, std::string(value.first, value.second) ) << FAS_TESTING_FILE_LINE;
}

BEGIN_SUITE(util, "")
  ADD_UNIT(util1)
  ADD_UNIT(util2)
END_SUITE(util)
