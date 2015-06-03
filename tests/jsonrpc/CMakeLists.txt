
message(STATUS "---++++++++++++++++++---")
message(STATUS "---${FAS_TESTING_CPP}---")

add_executable(benchmark1 benchmark.cpp)

add_executable(
  test_jsonrpc
  test_jsonrpc.cpp
  outgoing_suite.cpp
  method_suite.cpp
  handler_suite.cpp
  ${FAS_TESTING_CPP}
)

target_link_libraries(test_jsonrpc iow)
add_test( test_jsonrpc test_jsonrpc)