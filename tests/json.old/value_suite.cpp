#include <fas/testing.hpp>
#include <iow/json.old/json.hpp>
#include <algorithm>

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
    P operator() ( T& v, P beg, P end )
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



template<typename T, typename V>
void value_serializer_test(T& t, const V& v, const std::string& chk, int line)
{
  typedef typename ::iow::json::value<V>::serializer serializer_t;
  using namespace fas::testing;
  V val = v;
  std::string json;
  serializer_t()(val, std::back_inserter(json) );
  t << equal_str<expect>(json, chk) << "serialize: " << json << "!=" << chk << ", line: " << line;
  val = V();
  serializer_t()(val, json.begin(), json.end() );
  t << equal<expect>(v, val) << "unserialize: " << v << "!=" << val << ", line: " << line;
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
}


UNIT(float_unit, "")
{
  using namespace fas::testing;
  value_serializer_test<T, float>(t, 10.1, "10.1", __LINE__);
  value_serializer_test<T, double>(t, 10.1, "10.1", __LINE__);
}

UNIT(string_unit, "")
{
  using namespace fas::testing;
  value_serializer_test<T, std::string>(t, "привет", "\"привет\"", __LINE__);
  //value_serializer_test<T, std::wstring>(t, L"привет", L"\"привет1\"", __LINE__);
}



BEGIN_SUITE(value_suite, "")
  ADD_UNIT(bool_unit)
  ADD_UNIT(integer_unit)
  ADD_UNIT(float_unit)
  ADD_UNIT(string_unit)
END_SUITE(value_suite)