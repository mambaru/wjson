add_executable(data_test ${FAS_TESTING_CPP} data_test.cpp )

add_test(NAME data_test COMMAND data_test WORKING_DIRECTORY ${IOW_TEST_DIR})


