if ( NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
  message(STATUS "${PROJECT_NAME} is not top level project")
  return()
endif()

include(mambaopt)
include(mambalibs)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


