add_executable(benchmark benchmark.cpp)
add_executable(numbench  numbench.cpp)
add_executable(strbench  strbench.cpp)
add_executable(objarrbench  objarrbench.cpp)
add_executable(num  num.cpp)

set(
  test_source_list
  test_json.cpp
  value_suite.cpp
  array_suite.cpp
  object_suite.cpp
  dict_suite.cpp
  enum_suite.cpp
  util_suite.cpp
)

if ( NOT ${CMAKE_CXX_STANDARD} EQUAL 98 )
  set(
    test_source_list 
    ${test_source_list} 
    _json_suite.cpp
  )
endif()

add_executable(
  test_json
  ${test_source_list}
  ${FAS_TESTING_CPP}
)

add_test( NAME test_json COMMAND test_json WORKING_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})
if ( NOT BUILD_COVERAGE )
  add_test(NAME test_json-leak COMMAND valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./test_json WORKING_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})
endif()
