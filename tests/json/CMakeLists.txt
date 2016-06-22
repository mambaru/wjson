add_executable(benchmark benchmark.cpp)

add_executable(
  test_json
  test_json.cpp
  value_suite.cpp
  array_suite.cpp
  ${FAS_TESTING_CPP}
)

add_test( test_json test_json)