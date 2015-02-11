
#find_package( Threads )

#set(IOW_TEST_DIR ${CMAKE_BINARY_DIR}/test-bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})

add_executable(io_base_test io_base_test.cpp)
add_executable(descriptor_test descriptor_test.cpp)

target_link_libraries( io_base_test ${CMAKE_THREAD_LIBS_INIT}  ${Boost_LIBRARIES})
target_link_libraries( descriptor_test ${CMAKE_THREAD_LIBS_INIT}  ${Boost_LIBRARIES})

add_test(NAME io_base_test COMMAND io_base_test WORKING_DIRECTORY ${IOW_TEST_DIR})
add_test(NAME descriptor_test COMMAND descriptor_test WORKING_DIRECTORY ${IOW_TEST_DIR})


