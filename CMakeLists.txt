cmake_minimum_required(VERSION 2.6)

project(iow)

include_directories(.)
include_directories(../faslib)
include_directories(../../faslib)

SET(CMAKE_CXX_STANDARD    "-std=c++0x")
SET(CMAKE_CXX_FLAGS       "${CXX_FLAGS} ${CMAKE_CXX_STANDARD} ")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")

add_library(
  iow
  iow/logger/global_log.cpp
  iow/jsonrpc/incoming/incoming_holder.cpp
)

if ( IOW_BUILD_ALL )
  enable_testing()
  add_subdirectory(tests)
  #add_subdirectory(tutorial)
endif()


#add_subdirectory(tests/jsonrpc)