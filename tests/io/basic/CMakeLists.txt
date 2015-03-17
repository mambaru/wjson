add_executable(basic_test ${FAS_TESTING_CPP} basic_test.cpp io_base_suite.cpp)
target_link_libraries( basic_test ${CMAKE_THREAD_LIBS_INIT}  ${Boost_LIBRARIES})

add_test(NAME basic_test COMMAND basic_test WORKING_DIRECTORY ${IOW_TEST_DIR})


