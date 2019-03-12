if ( CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR )
  set(STANDALONE ON)
else()
  set(STANDALONE OFF)
endif()

if ( STANDALONE )

  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake-ci/cmake/ci.cmake)
    execute_process(
      COMMAND 
        git submodule update --init -- "external/cmake-ci"
      WORKING_DIRECTORY 
        ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE
        EXIT_CODE
    )
  endif()
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake-ci/cmake)
  include(opt)
  include(repos)
  include(getlibs)
endif()
