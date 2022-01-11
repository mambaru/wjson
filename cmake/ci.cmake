
if ( NOT IS_WCI_PROJECT )

  set(path "${PROJECT_SOURCE_DIR}")

  if(NOT EXISTS ${path}/external/cmake-ci/cmake/target.cmake)
    execute_process(
      COMMAND
        git submodule update --init -- "external/cmake-ci"
      WORKING_DIRECTORY
        ${path}
      RESULT_VARIABLE
        EXIT_CODE
      ERROR_QUIET
    )
    
    if ( ${EXIT_CODE} EQUAL 0 )
      set(IS_WCI_PROJECT TRUE)
    else()
      set(IS_WCI_PROJECT FALSE)
    endif()
  else()
    set(IS_WCI_PROJECT TRUE)
  endif()
  
  if ( IS_WCI_PROJECT )
    list(APPEND CMAKE_MODULE_PATH ${path}/external/cmake-ci/cmake)
    list(APPEND CMAKE_MODULE_PATH ${path}/cmake)
    include(cmake-ci)
    set( WCI_SUPERMODULE ON)
    set( WCI_DIR "${path}")
  endif()
  
endif(NOT IS_WCI_PROJECT)
