add_executable( owner_test ${FAS_TESTING_CPP} owner_test.cpp  )
target_link_libraries(owner_test iow)
add_test( NAME owner_test COMMAND owner_test WORKING_DIRECTORY ${IOW_TEST_DIR} )
