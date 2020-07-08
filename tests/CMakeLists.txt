link_libraries(faslib wjson)

add_executable(benchmark benchmark.cpp)
add_executable(numbench  numbench.cpp)
add_executable(strbench  strbench.cpp)
add_executable(objarrbench  objarrbench.cpp)
add_executable(num  num.cpp)

wci_utils(benchmark numbench strbench objarrbench num)

set(
  test_source_list
  test_json.cpp
    parser_suite.cpp
    value_suite.cpp
    array_suite.cpp
    object_suite.cpp
    dict_suite.cpp
    enum_suite.cpp
    util_suite.cpp
    time_suite.cpp
    _json_suite.cpp
)

add_executable(test_json ${test_source_list})
target_link_libraries(test_json PUBLIC fas::testing)
wci_test(NAME test_json TARGET test_json)



