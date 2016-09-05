add_executable(benchmark.old benchmark.cpp)

add_executable(
  test_json.old
  test_json.cpp
  value_suite.cpp
  ${FAS_TESTING_CPP}
)

add_test( test_json.old test_json.old)