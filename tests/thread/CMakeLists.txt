
set( SUITE_LIST
  delayed_queue_suite.cpp
  thread_pool_suite.cpp
)

add_executable( thread_test ${FAS_TESTING_CPP} thread_test.cpp ${SUITE_LIST} )
target_link_libraries(thread_test iow ${CMAKE_THREAD_LIBS_INIT} )
add_test( NAME thread_test COMMAND thread_test WORKING_DIRECTORY ${IOW_TEST_DIR} )
