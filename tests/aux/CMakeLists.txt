add_executable(write_buffer_test ${FAS_TESTING_CPP} write_buffer_test.cpp )
add_executable(read_buffer_test ${FAS_TESTING_CPP} read_buffer_test.cpp )
add_executable(write_buffer_bench write_buffer_bench.cpp )
add_executable(read_buffer_bench read_buffer_bench.cpp )

add_test(NAME write_buffer_test COMMAND write_buffer_test WORKING_DIRECTORY ${IOW_TEST_DIR})
add_test(NAME read_buffer_test COMMAND read_buffer_test WORKING_DIRECTORY ${IOW_TEST_DIR})


