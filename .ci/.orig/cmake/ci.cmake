if ( WCI_SUPERMODULE )
  set(WCI_TOPLEVEL ON)
elseif(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR )
  set(WCI_TOPLEVEL ON)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/external/cmake-ci/cmake/target.cmake)
  set(WCI_TOPLEVEL OFF)
else()
  set(WCI_TOPLEVEL OFF)

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

if ( WCI_TOPLEVEL AND NOT WCI_SUPERMODULE)

  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake-ci/cmake/target.cmake)
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
  include(repos)
  include(target)
  include(getlibs)
endif()

set(WCI_SUPERMODULE OFF)
