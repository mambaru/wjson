#include <fas/testing.hpp>
#include <wjson/wjson.hpp>

namespace {

template<typename V, V metric, typename T>
std::ostream& write_test(T& t, const V& value, const std::string& tjson)
{
  using namespace fas::testing;
  typename wjson::size_value<V, metric>::serializer size_ser;

  std::string json;
  size_ser(value, std::back_inserter(json));
  return t << equal<expect, std::string>(tjson, json);
}

template<typename V, V metric, typename T, typename X>
std::ostream& read_test(T& t, const std::string& json, const X& value)
{
  using namespace fas::testing;
  typename wjson::size_value<V, metric>::serializer size_ser;

  V svalue = 0;
  wjson::json_error e;
  size_ser(svalue, json.begin(), json.end(), &e);
  t << is_false<expect>(e.status()) << wjson::strerror::message_trace(e, json.begin(), json.end()) << FAS_FL;
  return t << equal<expect, X>(svalue, value);
}


UNIT(size1_write, "")
{
  using namespace fas::testing;
  t << nothing;
#if __cplusplus >= 201103L
  using namespace wjson::literals;
  write_test<size_t, 1>(t, 0, "0"_json) << FAS_FL;

  write_test<size_t, 1>(t, 1, "1"_json) << FAS_FL;
  write_test<size_t, 1>(t, 33, "33"_json) << FAS_FL;

  write_test<size_t, 1>(t, 1000, "'1KB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024, "'1KiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001, "1001"_json) << FAS_FL;

  write_test<size_t, 1>(t, 1000000, "'1MB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001000, "'1001KB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024*1024, "'1MiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024*1001, "'1001KiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001001, "1001001"_json) << FAS_FL;

  write_test<size_t, 1>(t, 1000000000, "'1GB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001000000, "'1001MB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001001000, "'1001001KB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024*1024*1024, "'1GiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024*1024*1001, "'1001MiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024*1001001, "'1001001KiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001001001, "1001001001"_json) << FAS_FL;

  write_test<size_t, 1>(t, 1000000000000, "'1TB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001000000000, "'1001GB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001001000000, "'1001001MB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001001001000, "'1001001001KB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024ul*1024*1024*1024, "'1TiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024ul*1024*1024*1001, "'1001GiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024ul*1024*1001001, "'1001001MiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1024ul*1001001001, "'1001001001KiB'"_json) << FAS_FL;
  write_test<size_t, 1>(t, 1001001001001, "1001001001001"_json) << FAS_FL;
#endif
}

UNIT(size1000_write, "")
{
  using namespace fas::testing;
  t << nothing;
#if __cplusplus >= 201103L
  using namespace wjson::literals;
  write_test<size_t, 1000>(t, 0, "'0KB'"_json) << FAS_FL;

  write_test<size_t, 1000>(t, 1, "'1KB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 33, "'33KB'"_json) << FAS_FL;

  write_test<size_t, 1000>(t, 1000, "'1MB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 1001, "'1001KB'"_json) << FAS_FL;

  write_test<size_t, 1000>(t, 1000000, "'1GB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 1001000, "'1001MB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 1001001, "'1001001KB'"_json) << FAS_FL;

  write_test<size_t, 1000>(t, 1000000000, "'1TB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 1001000000, "'1001GB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 1001001000, "'1001001MB'"_json) << FAS_FL;
  write_test<size_t, 1000>(t, 1001001001, "'1001001001KB'"_json) << FAS_FL;

  write_test<short, 1000>(t, 30, "'30KB'"_json) << FAS_FL;
#endif
}

UNIT(size1024_write, "")
{
  using namespace fas::testing;
  t << nothing;
#if __cplusplus >= 201103L
  using namespace wjson::literals;
  write_test<size_t, 1024>(t, 0, "'0KiB'"_json) << FAS_FL;

  write_test<size_t, 1024>(t, 1, "'1KiB'"_json) << FAS_FL;
  write_test<size_t, 1024>(t, 33, "'33KiB'"_json) << FAS_FL;

  write_test<size_t, 1024>(t, 1024, "'1MiB'"_json) << FAS_FL;
  write_test<size_t, 1024>(t, 1025, "'1025KiB'"_json) << FAS_FL;

  write_test<size_t, 1024>(t, 1024*1024,  "'1GiB'"_json) << FAS_FL;
  write_test<size_t, 1024>(t, 1024*1024*1024, "'1TiB'"_json) << FAS_FL;
  write_test<size_t, 1024>(t, 1024*1024*1024 - 73741824, "'1024GB'"_json) << FAS_FL;
#endif
}

UNIT(size1000000_write, "")
{
  using namespace fas::testing;
  t << nothing;
#if __cplusplus >= 201103L
  using namespace wjson::literals;
  write_test<size_t, 1000000>(t, 0, "'0MB'"_json) << FAS_FL;

  write_test<size_t, 1000000>(t, 1, "'1MB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 33, "'33MB'"_json) << FAS_FL;

  write_test<size_t, 1000000>(t, 1000, "'1GB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 1001, "'1001MB'"_json) << FAS_FL;

  write_test<size_t, 1000000>(t, 1000000, "'1TB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 1001000, "'1001GB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 1001001, "'1001001MB'"_json) << FAS_FL;

  write_test<size_t, 1000000>(t, 1000000000, "'1000TB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 1001000000, "'1001TB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 1001001000, "'1001001GB'"_json) << FAS_FL;
  write_test<size_t, 1000000>(t, 1001001001, "'1001001001MB'"_json) << FAS_FL;
#endif
}

UNIT(size1_read, "")
{
  using namespace fas::testing;
  t << nothing;
#if __cplusplus >= 201103L
  using namespace fas::testing;
  using namespace wjson::literals;

  read_test<size_t, 1>(t, "'10TiB'"_json, 10ul*1024*1024*1024*1024) << FAS_FL;
  read_test<size_t, 1>(t, "'10TB'"_json, 10000000000000ul) << FAS_FL;
  read_test<size_t, 1>(t, "'10GiB'"_json, 10ul*1024*1024*1024) << FAS_FL;
  read_test<size_t, 1>(t, "'10GB'"_json, 10000000000ul) << FAS_FL;
  read_test<size_t, 1>(t, "'10MiB'"_json, 10*1024*1024) << FAS_FL;
  read_test<size_t, 1>(t, "'10MB'"_json, 10000000) << FAS_FL;
  read_test<size_t, 1>(t, "'10KiB'"_json, 10*1024) << FAS_FL;
  read_test<size_t, 1>(t, "'10KB'"_json, 10000) << FAS_FL;
  read_test<size_t, 1>(t, "'10B'"_json, 10) << FAS_FL;
  read_test<size_t, 1>(t, "'10'"_json, 10) << FAS_FL;
  read_test<size_t, 1>(t, "10"_json, 10) << FAS_FL;

  // read_test<int, 1>(t, "'10TB'"_json, 10000000000000ul) << FAS_FL;
  read_test<int, 1>(t, "'1GB'"_json, 1000000000) << FAS_FL;
  read_test<int, 1>(t, "'10MB'"_json, 10000000) << FAS_FL;
  read_test<int, 1>(t, "'10KB'"_json, 10000) << FAS_FL;
  read_test<int, 1>(t, "'10B'"_json, 10) << FAS_FL;
  read_test<int, 1>(t, "'10'"_json, 10) << FAS_FL;
  read_test<int, 1>(t, "10"_json, 10) << FAS_FL;

  read_test<size_t, 1024>(t, "'10TiB'"_json, 10ul*1024*1024*1024) << FAS_FL;
  read_test<size_t, 1000>(t, "'10TB'"_json, 10000000000ul) << FAS_FL;

  read_test<size_t, 1024*1024>(t, "'10TiB'"_json, 10ul*1024*1024) << FAS_FL;
  read_test<size_t, 1000000>(t, "'10TB'"_json, 10000000) << FAS_FL;
#endif
}

}

BEGIN_SUITE(value_size, "")
  ADD_UNIT(size1_write)
  ADD_UNIT(size1000_write)
  ADD_UNIT(size1024_write)
  ADD_UNIT(size1000000_write)
  ADD_UNIT(size1_read)
END_SUITE(value_size)
