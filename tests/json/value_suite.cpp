#include <fas/testing.hpp>
#include <iow/json/json.hpp>
#include <iow/json/strerror.hpp>
#include <algorithm>

/*
namespace iow{ namespace json{


namespace detail
{
  template<typename T>
  class serializerF
  {
  public:
    template<typename P>
    P operator()( T v, P end)
    {
      std::stringstream ss;
      ss << v;
      auto str = ss.str();
      std::copy( str.begin(), str.end(), end );
      return end;
    }

    template<typename P>
    P operator() ( T& v, P beg, P end, iow::json::json_error*  )
    {
      std::stringstream ss;
      ss << std::string(beg, end);
      ss >> v;
      return end; //!!! нет
    }
  };
}

template<>
class serializerT< value<float> >
  : public detail::serializerF<float>
{
};

template<>
class serializerT< value<double> >
  : public detail::serializerF<double>
{
};

template<>
struct value<float>
{
  typedef float target;
  typedef serializerT< value<float> > serializer;
};

template<>
struct value<double>
{
  typedef double target;
  typedef serializerT< value<double> > serializer;
};


}}
*/


template<typename T, typename V, typename VV>
void value_serializer_test(T& t, const VV& v, const std::string& chk, int line)
{
  typedef typename ::iow::json::value<V>::serializer serializer_t;
  using namespace fas::testing;
  V val = static_cast<V>(v);
  std::string json;
  serializer_t()(val, std::back_inserter(json) );
  t << equal_str<expect>(json, chk) << "Serialize. Line: " << line;
  val = V();
  ::iow::json::json_error e;
  serializer_t()(val, json.begin(), json.end(), &e );
  t << equal<expect, V>(v, val) << "Unserialize. Line: " << line;
}

template<typename T, typename V, int R>
void real_serializer_test(T& t, const V& v, const std::string& chk, int line)
{
  typedef typename ::iow::json::value<V, R>::serializer serializer_t;
  using namespace fas::testing;
  V val = v;
  std::string json;
  serializer_t()(val, std::back_inserter(json) );
  t << equal_str<expect>(json, chk) << "Serialize. Line: " << line;
  val = V();
  ::iow::json::json_error e;
  serializer_t()(val, json.begin(), json.end(), &e );
  t << less<expect>(v - val, 0.000001) << "Unserialize. Line: " << line;
}

UNIT(bool_unit, "")
{
 value_serializer_test<T, bool>(t, true, "true", __LINE__);
 value_serializer_test<T, bool>(t, false, "false", __LINE__);
}

UNIT(integer_unit, "")
{
  using namespace fas::testing;
  value_serializer_test<T, char>(t, 10, "10", __LINE__);
  value_serializer_test<T, unsigned char>(t, 10, "10", __LINE__);
  value_serializer_test<T, short>(t, 10, "10", __LINE__);
  value_serializer_test<T, unsigned short>(t, 10, "10", __LINE__);
  value_serializer_test<T, int>(t, 10, "10", __LINE__);
  value_serializer_test<T, unsigned int>(t, 10, "10", __LINE__);
  value_serializer_test<T, long>(t, 1199511627775, "1199511627775", __LINE__);
  value_serializer_test<T, unsigned long>(t, 1199511627775, "1199511627775", __LINE__);
  value_serializer_test<T, time_t>(t, 1199511627775, "1199511627775", __LINE__);
  value_serializer_test<T, size_t>(t, 1199511627775, "1199511627775", __LINE__);
  value_serializer_test<T, std::ptrdiff_t>(t, 1199511627775, "1199511627775", __LINE__);

  value_serializer_test<T, char>(t, -10, "-10", __LINE__);
  value_serializer_test<T, unsigned char>(t, -10, "246", __LINE__);
  value_serializer_test<T, short>(t, -10, "-10", __LINE__);
  value_serializer_test<T, unsigned short>(t, -10, "65526", __LINE__);
  value_serializer_test<T, int>(t, -10, "-10", __LINE__);
  value_serializer_test<T, unsigned int>(t, -10, "4294967286", __LINE__);
  value_serializer_test<T, long>(t, -1199511627775, "-1199511627775", __LINE__);
  value_serializer_test<T, unsigned long>(t, -1199511627775, "18446742874197923841", __LINE__);
  value_serializer_test<T, time_t>(t, -1199511627775, "-1199511627775", __LINE__);
  value_serializer_test<T, size_t>(t, -1199511627775, "18446742874197923841", __LINE__);
  value_serializer_test<T, std::ptrdiff_t>(t, -1199511627775, "-1199511627775", __LINE__);

}

UNIT(float_unit, "")
{
  using namespace fas::testing;
  real_serializer_test<T, float, -1>(t, 10.1, "1.010000e+01", __LINE__);
  real_serializer_test<T, double, -1>(t, 10.1, "1.010000e+01", __LINE__);
  real_serializer_test<T, long double, -1>(t, 10.1, "1.010000e+01", __LINE__);

  real_serializer_test<T, float, 0>(t, 10.0, "10", __LINE__);
  real_serializer_test<T, double, 2>(t, 10.1, "10.10", __LINE__);
  real_serializer_test<T, long double, 10>(t, 10.1, "10.1000000000", __LINE__);
  
}

UNIT(string_unit, "")
{
  using namespace fas::testing;
  value_serializer_test<T, std::string>(t, "привет", "\"привет\"", __LINE__);
}

UNIT(string1, "" )
{
  using namespace fas::testing;
  using namespace iow::json;
  std::string str="hello world! Привет мир!";
  str.push_back(static_cast<char>(132));
  str.push_back(static_cast<char>(131));
  str+="世界你好!";
  std::vector<char> vstr(str.begin(), str.end() );
  vstr[5]='\0';
  vstr[12]=static_cast<char>(130);
  std::string json, json2;
  value< std::vector<char> >::serializer()(vstr, std::back_inserter(json));
  json2="\"hello\\u0000world!\\x82Привет мир!\\x84\\x83世界你好!\"";
  t << equal<expect>(json.size(), json2.size()) << FAS_FL;
  t << equal<expect>(json, json2) << FAS_FL;
  std::string str2;
  json_error e;
  value< std::string >::serializer()(str2, json.begin(), json.end(), &e );
  t << is_false<expect>(e) << strerror::message_trace(e, json.begin(), json.end() ) << FAS_FL;
  t << equal<expect>(str2, str) << FAS_FL;
  //t << equal<expect>(str2, "hello\\u0000world!\\u0082Привет мир!\\u8384世界你好!") << FAS_FL;
  
  json = "\"\u4E16\u754C\u4F60\u597D\"";
  value< std::string >::serializer()(str, json.begin(), json.end(), &e );
  t << equal<expect>(str, "世界你好") << FAS_FL;
}

BEGIN_SUITE(value, "")
  ADD_UNIT(bool_unit)
  ADD_UNIT(integer_unit)
  ADD_UNIT(float_unit)
  ADD_UNIT(string_unit)
  ADD_UNIT(string1)
END_SUITE(value)