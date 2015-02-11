cmake_minimum_required(VERSION 2.6)

project(iow)

include_directories(.)
include_directories(../faslib)

add_subdirectory(tests)

if ( WFC_BUILD_ALL )
  #add_subdirectory(tests)
  #add_subdirectory(tutorial)
endif()

