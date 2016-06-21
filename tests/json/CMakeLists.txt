add_executable(benchmark1 benchmark.cpp)

add_executable(
  test_json
  test_json.cpp
  value_suite.cpp
  ${FAS_TESTING_CPP}
)

add_test( test_json test_json)