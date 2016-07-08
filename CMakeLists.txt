cmake_minimum_required(VERSION 2.6)

project(wjson)

if ( IOW_BUILD_TEST OR IOW_BUILD_ALL )
  if( IOW_BUILD_TEST==ON )
    enable_testing()
    add_subdirectory(tests)
  endif() # IOW_BUILD_TEST
  add_subdirectory(examples)
endif()

