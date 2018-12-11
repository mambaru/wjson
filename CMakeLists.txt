cmake_minimum_required(VERSION 3.1)
project(wjson)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

if ( STANDALONE )
  getlibs(faslib)
endif()

if ( BUILD_TESTING )
  enable_testing()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})
  include_directories(.)
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

set(CPACK_PACKAGE_NAME "wjson")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "7")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_PACKAGE_CONTACT "migashko@gmail.com")

install(DIRECTORY wjson DESTINATION include
  PATTERN "*.cmake" EXCLUDE
  PATTERN "*~" EXCLUDE
  PATTERN ".*" EXCLUDE
  PATTERN "Makefile" EXCLUDE
  PATTERN "CMakeFiles" EXCLUDE
)

install(DIRECTORY . DESTINATION share/wjson
  PATTERN "wjson" EXCLUDE
  PATTERN "*.cmake" EXCLUDE
  PATTERN "*~*" EXCLUDE
  PATTERN "*kdev*" EXCLUDE
  PATTERN ".git*" EXCLUDE
  PATTERN "Readme.md" EXCLUDE
)

include(CPack)
