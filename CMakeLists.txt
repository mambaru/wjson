cmake_minimum_required(VERSION 2.6)

project(wjson)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

