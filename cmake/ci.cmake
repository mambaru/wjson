MACRO(wci_init path)

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
ENDMACRO()

MACRO(wci_cmake)
  wci_init(${CMAKE_CURRENT_SOURCE_DIR})
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake-ci/cmake)
  include(repos)
  include(options)
  include(target)
  include(getlibs)
ENDMACRO()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR )
  wci_cmake()
  set(WCI_SUPERMODULE "SUPERMODULE")
else()
  wci_init(${CMAKE_SOURCE_DIR})
  if (NOT IS_WCI_PROJECT )
    if ( WCI_SUPERMODULE )
      wci_cmake()
    elseif( NOT WCI_SUBSUB)
      macro(wci_getlib)
      endmacro()
      macro(wci_test)
      endmacro()
      macro(wci_targets)
      endmacro()
      macro(wci_utils)
      endmacro()
      macro(wci_add_options)
      endmacro()
      macro(wci_remove_options)
      endmacro()
    endif()
    set(WCI_SUBSUB TRUE)
  endif()
endif()
