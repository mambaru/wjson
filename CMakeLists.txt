cmake_minimum_required(VERSION 2.8)

project(wjson)

if ( BUILD_TESTING )
  set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  include(WJSON)
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()
