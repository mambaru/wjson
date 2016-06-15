
set( SUITE_LIST
  delayed_queue_suite.cpp
  deadline_timer_suite.cpp
  thread_pool_suite.cpp
  timer_manager_suite.cpp
  workflow_suite.cpp
)

add_executable( workflow_test ${FAS_TESTING_CPP} workflow_test.cpp ${SUITE_LIST} )
target_link_libraries(workflow_test iow ${CMAKE_THREAD_LIBS_INIT} )
add_test( NAME workflow_test COMMAND workflow_test WORKING_DIRECTORY ${IOW_TEST_DIR} )
