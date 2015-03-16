add_executable(proactor_test ${FAS_TESTING_CPP} proactor_test.cpp rnstream_suite.cpp)
target_link_libraries( proactor_test ${CMAKE_THREAD_LIBS_INIT}  ${Boost_LIBRARIES})

add_test(NAME proactor_test COMMAND proactor_test WORKING_DIRECTORY ${IOW_TEST_DIR})


