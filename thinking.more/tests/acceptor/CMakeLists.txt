add_executable(acceptor_test ${FAS_TESTING_CPP} acceptor_test.cpp )
target_link_libraries( acceptor_test ${CMAKE_THREAD_LIBS_INIT}  ${Boost_LIBRARIES})

add_test(NAME acceptor_test COMMAND acceptor_test WORKING_DIRECTORY ${IOW_TEST_DIR})


