cmake_minimum_required(VERSION 2.6)

project(wjson)

if ( WJSON_BUILD_TEST OR WJSON_BUILD_ALL )
  add_subdirectory(tests)
  if( WJSON_BUILD_ALL )
    add_subdirectory(examples)
  endif() # WJSON_BUILD_TEST
endif()

