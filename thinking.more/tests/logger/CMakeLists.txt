
#find_package( Threads )

#set(IOW_TEST_DIR ${CMAKE_BINARY_DIR}/test-bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})


add_executable(logstream_test logstream_test.cpp)
add_executable(logger_test logger_test.cpp ../../iow/logger/global_log.cpp)

add_test(NAME logstream_test COMMAND logstream_test WORKING_DIRECTORY ${IOW_TEST_DIR})
add_test(NAME logger_test COMMAND logger_test WORKING_DIRECTORY ${IOW_TEST_DIR})

