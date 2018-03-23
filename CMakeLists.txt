cmake_minimum_required(VERSION 2.8)
project(wjson)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(WJSON)

include_directories(.)
if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

set(CPACK_PACKAGE_NAME "wjson")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "7")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_CONTACT "migashko@gmail.com")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
  PATTERN "README.md" EXCLUDE
)

include(CPack)
