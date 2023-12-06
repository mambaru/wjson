cmake_minimum_required(VERSION 3.10)

project(wjson VERSION "0.9.8")

include(cmake/ci.cmake)

wci_submodule(NAME faslib)

add_library(wjson INTERFACE)

target_include_directories(wjson INTERFACE $<BUILD_INTERFACE:${wjson_SOURCE_DIR}>)

target_link_libraries(wjson INTERFACE faslib)

if ( BUILD_TESTING OR CODE_COVERAGE)
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
  endif()
endif()

include(cmake/install.cmake)
