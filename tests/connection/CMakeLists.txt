add_executable(connection_test ${FAS_TESTING_CPP} connection_test.cpp )
target_link_libraries( connection_test ${CMAKE_THREAD_LIBS_INIT}  ${Boost_LIBRARIES})

add_test(NAME connection_test COMMAND connection_test WORKING_DIRECTORY ${IOW_TEST_DIR})


