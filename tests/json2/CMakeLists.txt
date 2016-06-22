add_executable(benchmark2 benchmark.cpp)

add_executable(
  test_json2
  test_json.cpp
  value_suite.cpp
  ${FAS_TESTING_CPP}
)

add_test( test2_json test2_json)