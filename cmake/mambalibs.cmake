# ConfigureLibrary.cmake
#
# Configure library
#

MACRO(CLONE_LIBRARY LIBNAME VARDIR LIBURI PARAMS_FOR_CMAKE)
  if ( NOT "${VARDIR}" )
    if ( HAVE_INCLUDE_${LIBNAME} )
      set(${VARDIR} "${HAVE_INCLUDE_${LIBNAME}}")
    else()
      if ( NOT EXISTS "${CMAKE_SOURCE_DIR}/build" )
        execute_process(COMMAND mkdir build WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
      endif()
      if ( NOT EXISTS "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        execute_process(COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE current_branch )
        execute_process(COMMAND git clone "${LIBURI}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
        if ( "${current_branch}" MATCHES "devel" )
          message(STATUS "DEBUG VERSION! checkout devel!!!")
          execute_process(COMMAND git checkout devel WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        endif()
        
        execute_process(COMMAND mkdir build WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        execute_process(COMMAND cmake ${PARAMS_FOR_CMAKE1} ..  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}/build")
        execute_process(COMMAND cmake --build . --config Release WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}/build")
      endif()
      set(${VARDIR} "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
    endif()
  endif( NOT "${VARDIR}" )
ENDMACRO(CLONE_LIBRARY)

MACRO(get_faslib)
  unset(FASLIB_DIR CACHE)
  find_path( 
    FASLIB_DIR NAMES "fas/aop.hpp"
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" 
    PATH_SUFFIXES 
      "build/faslib" 
      "faslib" 
      "../faslib" 
      "../../faslib" 
      "../../../faslib" 
  )
  if ( "${FASLIB_DIR}" STREQUAL "FASLIB_DIR-NOTFOUND") 
    unset(FASLIB_DIR CACHE)
    execute_process(COMMAND bash -c "git remote -v | head -q -n 1" 
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE remote_url )
    if ( "${remote_url}" MATCHES "\.lan" )
      message(STATUS "Clone from LAN repositary")
      clone_library(faslib "FASLIB_DIR" "git@github.lan:cpp/faslib.git" "")
    else()
      message(STATUS "Clone from github.com")
      clone_library(faslib "FASLIB_DIR" "https://github.com/migashko/faslib.git" "")
    endif()
  endif()
  include_directories("${FASLIB_DIR}")
  set(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")
ENDMACRO(get_faslib)

MACRO(get_mambaru LIBNAME LIBDIR LIBBIN PARAMS_FOR_CMAKE)
  unset(${${LIBDIR}} CACHE)
  find_path( 
    ${LIBDIR} NAMES "${LIBNAME}/${LIBNAME}.hpp"
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" 
    PATH_SUFFIXES
      "build/${LIBNAME}" 
      "${LIBNAME}" 
      "../${LIBNAME}"  
      "../../${LIBNAME}" 
      "../../../${LIBNAME}" 
  )
  if ( "${${LIBDIR}}" STREQUAL "${LIBDIR}-NOTFOUND") 
    unset(${LIBDIR} CACHE)
    execute_process(COMMAND bash -c "git remote -v | head -q -n 1" 
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE remote_url )
    message(STATUS "remote_url=${remote_url}")
    if ( "${remote_url}" MATCHES "github\.lan" )
      message(STATUS "Clone from LAN repositary")
      clone_library(${LIBNAME} ${LIBDIR} "git@github.lan:cpp/${LIBNAME}.git" "${PARAMS_FOR_CMAKE}")
    else()
      message(STATUS "Clone from github.com")
      clone_library(${LIBNAME} ${LIBDIR} "https://github.com/mambaru/${LIBNAME}.git" "${PARAMS_FOR_CMAKE}")
    endif()
  endif()
  MESSAGE(STATUS "${LIBDIR} = ${${LIBDIR}}")
  include_directories("${${LIBDIR}}")
  
  unset(${${LIBBIN}} CACHE)
  find_library( 
    ${LIBBIN} NAMES "${LIBNAME}"
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" 
    PATH_SUFFIXES 
      "build/${LIBNAME}/build" 
      "${LIBNAME}/build" 
      "../${LIBNAME}/build" 
      "../../${LIBNAME}/build" 
      "../../../${LIBNAME}/build"
  )
  if ( ${${LIBBIN}} )
    link_directories("${${LIBBIN}}")
    MESSAGE(STATUS "${LIBBIN} = ${${LIBBIN}}")
  endif()
ENDMACRO(get_mambaru)

