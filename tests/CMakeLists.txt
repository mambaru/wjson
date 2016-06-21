enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})

SET(CMAKE_CXX_FLAGS "-std=c++11")

if ( NOT FAS_TESTING_CPP )
  set( FAS_TESTING_CPP ${CMAKE_SOURCE_DIR}/../faslib/fas/testing/testing.cpp )
  include_directories(${CMAKE_SOURCE_DIR})
  include_directories(${CMAKE_SOURCE_DIR}/../faslib)
endif()


add_subdirectory(json)
