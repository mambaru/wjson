#include <fas/testing.hpp>
#include <iow/json/json.hpp>
#include <algorithm>
#include <cstring>

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




UNIT(util3, "member_value")
{
  using namespace fas::testing;
  using namespace iow::json;
  
  struct foo
  {
    int value = 12345;
  };
  
  struct bar: foo{};
  
  bar b;
  
  std::string json;
  typename member_value< bar, foo, int, &foo::value, value<int> >::serializer ser;
  ser( b, std::back_inserter(json) );
  
  t << equal<expect>( json, "12345" ) << FAS_TESTING_FILE_LINE;
  std::reverse(json.begin(), json.end());
  ser( b, json.begin(), json.end(), 0 );
  t << equal<expect>( b.value, 54321 ) << FAS_TESTING_FILE_LINE;
}

UNIT(util4, "pointer")
{
  using namespace fas::testing;
  using namespace iow::json;
  
  {
    std::string *str = new std::string;
    std::string json;
    pointer< std::string*, value< std::string > >::serializer ser;
    // ser( str, json.begin(), json.end(), 0 );
    *str = "Привет мир!";
    ser(str, std::back_inserter( json ) );
    t << equal<expect>( json, "\"Привет мир!\"" ) << FAS_TESTING_FILE_LINE;
    delete str;
  }

  {
    std::string json;
    pointer< std::shared_ptr<std::string>, value< std::string > >::serializer ser;
    auto str = std::make_shared<std::string>("Привет мир!");
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "\"Привет мир!\"" ) << FAS_TESTING_FILE_LINE;
    str = nullptr;
    ser( str, json.begin(), json.end(), 0 );
    t << equal<expect>( *str, "Привет мир!" ) << FAS_TESTING_FILE_LINE;
    json = "null";
    ser( str, json.begin(), json.end(), 0 );
    t << equal<expect>( str, nullptr ) << FAS_TESTING_FILE_LINE;
    json.clear();
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "null" ) << FAS_TESTING_FILE_LINE;
  }

  {
    std::string json;
    pointer< std::unique_ptr<std::string>, value< std::string > >::serializer ser;
    std::unique_ptr<std::string> str( new std::string("Привет мир!") );
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "\"Привет мир!\"" ) << FAS_TESTING_FILE_LINE;
    str = nullptr;
    ser( str, json.begin(), json.end(), 0 );
    t << equal<expect>( *str, "Привет мир!" ) << FAS_TESTING_FILE_LINE;
    json = "null";
    ser( str, json.begin(), json.end(), 0 );
    t << is_true<expect>( str == nullptr ) << FAS_TESTING_FILE_LINE;
    json.clear();
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "null" ) << FAS_TESTING_FILE_LINE;
  }

}

UNIT(util5, "pair")
{
  using namespace fas::testing;
  using namespace iow::json;
  
  std::pair<std::string, int> p("foo", 12345 );
  pair< value< std::string>, value<int> >::serializer ser;
  std::string json;
  ser( p, std::back_inserter(json) );
  t << equal<expect>( json, "\"foo\":12345") << FAS_TESTING_FILE_LINE;
}

BEGIN_SUITE(util, "")
  ADD_UNIT(util1)
  ADD_UNIT(util2)
  ADD_UNIT(util3)
  ADD_UNIT(util4)
  ADD_UNIT(util5)
END_SUITE(util)
