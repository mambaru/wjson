#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <fas/testing.hpp>
#include <fas/system/nullptr.hpp>
#include <algorithm>
#include <cstring>

UNIT(errors, "errors")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string any = "1,2,3,4,5";
  json_error je;
  create_error<error_code::ExpectedOf>(&je);
  je.reset();
  create_error<error_code::ExpectedOf>(&je, any.begin(), ";", 3 );
  t << equal<expect, int>(je.tail_of(), 3) << FAS_FL;
  t << equal<expect, int>(je.code(), error_code::ExpectedOf) << FAS_FL;

  std::string err = wjson::strerror::message_trace(je, any.begin(), any.end());
  wjson::error_code_json::serializer()( error_code::InvalidRequest, std::back_inserter(any) );
  wjson::error_code_json::serializer()( error_code::InvalidJSON, std::back_inserter(any) );
  wjson::error_code_json::serializer()( error_code::InvalidNull, std::back_inserter(any) );
  wjson::error_code_json::serializer()( error_code::InvalidBool, std::back_inserter(any) );
  wjson::error_code_json::serializer()( error_code::InvalidString, std::back_inserter(any) );
  wjson::error_code_json::serializer()( error_code::InvalidMember, std::back_inserter(any) );

  je.reset();
  any.clear();

  create_error<error_code::InvalidRequest>(&je, any.begin(), "l", 100);
  t << equal<expect, int>(je.tail_of(), 100) << FAS_FL;
  size_t whe = wjson::strerror::where( je, any.begin(), any.end() );
  t << equal<expect, int>(je.tail_of(), 100) << FAS_FL;
  t << equal<expect, int>(whe, 0) << FAS_FL;
  t << message(err) << any;
  const char* perr = wjson::strerror::what(je);
  t << equal<expect, std::string>(perr, "Invalid JSON-RPC Request") << FAS_FL;

}

UNIT(util1, "raw_value")
{
  using namespace fas::testing;
  using namespace wjson;

  std::string json="\"Привет мир!\"";
  std::string value;
  raw_value< std::string >::serializer ser;
  ser( value, json.begin(), json.end(), fas_nullptr);
  t << equal<expect>(json, value) << FAS_FL;
  json.clear();;
  ser( value, std::back_inserter(json) );
  t << equal<expect>(json, value) << FAS_FL;
}

UNIT(util2, "raw_range")
{
  using namespace fas::testing;
  using namespace wjson;

  std::string json="\"Привет мир!\"";
  typedef std::pair<std::string::iterator, std::string::iterator> range_t;
  range_t value;
  iterator_pair<range_t>::serializer ser;
  ser( value, json.begin(), json.end(), fas_nullptr );
  t << equal<expect>(json, std::string(value.first, value.second)) << FAS_FL;
  json.clear();
    ser( value, std::back_inserter(json) );
  t << equal<expect>(json, std::string(value.first, value.second) ) << FAS_FL;
}
namespace
{
  struct foo
  {
    int value;
    foo():value(12345){}
  };

  struct bar: foo{};
}

UNIT(util3, "member_value")
{
  using namespace fas::testing;
  using namespace wjson;


  bar b;

  std::string json;
  typename member_value< bar, foo, int, &foo::value, value<int> >::serializer ser;
  ser( b, std::back_inserter(json) );

  t << equal<expect>( json, "12345" ) << FAS_FL;
  std::reverse(json.begin(), json.end());
  ser( b, json.begin(), json.end(), fas_nullptr );
  t << equal<expect>( b.value, 54321 ) << FAS_FL;
}

UNIT(util4, "pointer")
{
  using namespace fas::testing;
  using namespace wjson;

  {
    std::string *str = new std::string;
    std::string json;
    pointer< std::string*, value< std::string > >::serializer ser;
    // ser( str, json.begin(), json.end(), 0 );
    *str = "Привет мир!";
    ser(str, std::back_inserter( json ) );
    t << equal<expect>( json, "\"Привет мир!\"" ) << FAS_FL;
    delete str;
  }

#if __cplusplus >= 201103L
  {
    std::string json;
    pointer< std::shared_ptr<std::string>, value< std::string > >::serializer ser;
    auto str = std::make_shared<std::string>("Привет мир!");
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "\"Привет мир!\"" ) << FAS_FL;
    str = fas_nullptr;
    ser( str, json.begin(), json.end(), fas_nullptr );
    t << equal<expect>( *str, "Привет мир!" ) << FAS_FL;
    json = "null";
    ser( str, json.begin(), json.end(), fas_nullptr );
    t << equal<expect>( str, fas_nullptr ) << FAS_FL;
    json.clear();
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "null" ) << FAS_FL;
  }


  {
    std::string json;
    pointer< std::unique_ptr<std::string>, value< std::string > >::serializer ser;
    std::unique_ptr<std::string> str( new std::string("Привет мир!") );
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "\"Привет мир!\"" ) << FAS_FL;
    str = fas_nullptr;
    ser( str, json.begin(), json.end(), fas_nullptr );
    t << equal<expect>( *str, "Привет мир!" ) << FAS_FL;
    json = "null";
    ser( str, json.begin(), json.end(), fas_nullptr );
    t << is_true<expect>( str == fas_nullptr ) << FAS_FL;
    json.clear();
    ser( str, std::back_inserter( json ) );
    t << equal<expect>( json, "null" ) << FAS_FL;
  }
#endif

}

UNIT(util5, "pair")
{
  using namespace fas::testing;
  using namespace wjson;

  std::pair<std::string, int> p("foo", 12345 );
  field< value< std::string>, value<int> >::serializer ser;
  std::string json;
  ser( p, std::back_inserter(json) );
  t << equal<expect>( json, "\"foo\":12345") << FAS_FL;
}

UNIT(util6, "quoted")
{
  using namespace fas::testing;
  using namespace wjson;
  int val = 123456;
  std::string json;
  typedef value<int> int_json;
  quoted<int_json>::serializer()( val, std::back_inserter(json) );
  t << equal<expect>( json, "\"123456\"") << FAS_FL;
  val=0;
  quoted<int_json>::serializer()( val, json.begin(), json.end(), fas_nullptr );
  t << equal<expect>( val, 123456) <<  FAS_FL;

  val=123456;json.clear();
  quoted<int_json, false, false, 1>::serializer()( val, std::back_inserter(json) );
  t << equal<expect>( json, "123456") << FAS_FL;
  val=0;
  quoted<int_json, false, false, 1>::serializer()( val, json.begin(), json.end(), fas_nullptr );
  t << equal<expect>( val, 123456) <<  FAS_FL;

  std::vector<int> arr;
  arr.push_back(1);arr.push_back(2);arr.push_back(3);
  typedef array< std::vector< value<int> > > arr_json;
  json.clear();
  quoted<arr_json>::serializer()( arr, std::back_inserter(json) );
  t << equal<expect>( json, "\"[1,2,3]\"") << FAS_FL;
  arr.clear();
  quoted<arr_json>::serializer()( arr, json.begin(), json.end(), fas_nullptr );
#if __cplusplus >= 201103L
  t << equal<expect>( arr, std::vector<int>{1,2,3} ) <<  FAS_FL;
#endif

  json = "\"[1,[1,2,3,4],3]\"";
  json_error e;
  arr.clear();
    quoted<arr_json>::serializer()( arr, json.begin(), json.end(), &e );
  t << message("error: ") << strerror::message(e);
  t << message("trace: ") << strerror::trace(e, json.begin(), json.end());
  t << is_true<expect>(e) << FAS_FL;
}

UNIT(util7, "raw_quoted")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string val = "123456";
  std::string json;

  raw_quoted<std::string>::serializer()( val, std::back_inserter(json) );
  t << equal<expect>( json, "\"123456\"") << FAS_FL;
  val="0";
  json_error e;
  raw_quoted<std::string>::serializer()( val, json.begin(), json.end(), &e );
  t << is_false<assert>(e) << strerror::message(e) << FAS_FL;
  t << equal<expect>( val, "123456") <<  FAS_FL;

  val="123456";
  json.clear();
  raw_quoted<std::string, false, false, 1>::serializer()( val, std::back_inserter(json) );
  t << equal<expect>( json, "123456") << FAS_FL;

  val="0";
  raw_quoted<std::string, false, false, 1>::serializer()( val, json.begin(), json.end(), &e );
  t << is_false<assert>(e) << strerror::message(e) << FAS_FL;
  t << equal<expect, std::string>( val, "123456") <<  FAS_FL;

  std::string arr="[1,2,3]";
  json.clear();
  raw_quoted<std::string>::serializer()( arr, std::back_inserter(json) );
  t << equal<expect>( json, "\"[1,2,3]\"") << FAS_FL;
  arr.clear();
  raw_quoted<std::string>::serializer()( arr, json.begin(), json.end(), &e );
  t << is_false<assert>(e) << strerror::message(e) << FAS_FL;
#if __cplusplus >= 201103L
  t << equal<expect>( arr, "[1,2,3]" ) <<  FAS_FL;
#endif

  json = "\"[1,[1,2,3,4],3]\"";

  arr.clear();
  raw_quoted<std::string>::serializer()( arr, json.begin(), json.end(), &e );
  t << is_false<assert>(e) << strerror::message(e) << FAS_FL;
  t << equal<expect>( arr, "[1,[1,2,3,4],3]") << FAS_FL;
}

BEGIN_SUITE(util, "")
  ADD_UNIT(errors)
  ADD_UNIT(util1)
  ADD_UNIT(util2)
  ADD_UNIT(util3)
  ADD_UNIT(util4)
  ADD_UNIT(util5)
  ADD_UNIT(util6)
  ADD_UNIT(util7)
END_SUITE(util)
