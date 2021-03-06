#include <fas/testing.hpp>

BEGIN_TEST
  RUN_SUITE(parser)
  RUN_SUITE(value)
  RUN_SUITE(array)
  RUN_SUITE(object)
  RUN_SUITE(dict)
  RUN_SUITE(enumerator)
  RUN_SUITE(util)
  RUN_SUITE(time_interval)
  RUN_SUITE(value_size)
  RUN_SUITE(schema)
#if __cplusplus >= 201103L
  RUN_SUITE(_json)
#endif
END_TEST
