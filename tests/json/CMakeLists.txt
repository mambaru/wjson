add_executable(benchmark benchmark.cpp)
add_executable(numbench  numbench.cpp)
add_executable(strbench  strbench.cpp)
add_executable(objarrbench  objarrbench.cpp)
add_executable(num  num.cpp)

add_executable(
  test_json
  test_json.cpp
  value_suite.cpp
  array_suite.cpp
  object_suite.cpp
  dict_suite.cpp
  enum_suite.cpp
  util_suite.cpp
  ${FAS_TESTING_CPP}
)

add_test( test_json test_json)