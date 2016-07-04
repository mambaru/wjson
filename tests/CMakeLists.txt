enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})

SET(CMAKE_CXX_FLAGS "-std=c++11 -W -Wall -Werror -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")


if ( NOT FAS_TESTING_CPP )
  set( FAS_TESTING_CPP ${CMAKE_SOURCE_DIR}/../faslib/fas/testing/testing.cpp )
  include_directories(${CMAKE_SOURCE_DIR})
  include_directories(${CMAKE_SOURCE_DIR}/../faslib)
endif()


add_subdirectory(json)
# add_subdirectory(json.old)

