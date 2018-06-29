# ConfigureLibrary.cmake
#
# Configure library
#

MACRO(CLONE_LIBRARY LIBNAME VARDIR LIBURI )
  if ( NOT "${VARDIR}" )
    if ( HAVE_INCLUDE_${LIBNAME} )
      set(${VARDIR} "${HAVE_INCLUDE_${LIBNAME}}")
    else()
      if ( NOT EXISTS "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        set(store_BUILD_TESTING ${BUILD_TESTING})
        unset(BUILD_TESTING)
        execute_process(COMMAND git clone "${LIBURI}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
        message(COMMAND "DEBUG VERSION! checkout devel!!!")
        execute_process(COMMAND git checkout devel WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        execute_process(COMMAND cmake . WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        execute_process(COMMAND make WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${LIBNAME}")
        set(BUILD_TESTING ${store_BUILD_TESTING})
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
    PATH_SUFFIXES "../faslib" "faslib" "build/faslib" "../build/faslib" 
  )
  if ( "${FASLIB_DIR}" STREQUAL "FASLIB_DIR-NOTFOUND") 
    unset(FASLIB_DIR CACHE)
    clone_library(faslib "FASLIB_DIR" "https://github.com/migashko/faslib.git")
  endif()
  include_directories("${FASLIB_DIR}")
  set(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")
ENDMACRO(get_faslib)

MACRO(get_mambaru LIBNAME LIBDIR LIBBIN)
  unset(${${LIBDIR}} CACHE)
  find_path( 
    ${LIBDIR} NAMES "${LIBNAME}/${LIBNAME}.hpp"
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" 
    PATH_SUFFIXES "build/${LIBNAME}" "../build/${LIBNAME}" "${LIBNAME}" "../${LIBNAME}"
  )
  if ( "${${LIBDIR}}" STREQUAL "${LIBDIR}-NOTFOUND") 
    unset(${LIBDIR} CACHE)
    #clone_library(${LIBNAME} ${LIBDIR} "https://github.com/mambaru/${LIBNAME}.git")
    clone_library(${LIBNAME} ${LIBDIR} "git@github.lan:cpp/${LIBNAME}.git")
  endif()
  MESSAGE(STATUS "${LIBDIR} = ${${LIBDIR}}")
  include_directories("${${LIBDIR}}")
  
  unset(${${LIBBIN}} CACHE)
  find_library( 
    ${LIBBIN} NAMES "${LIBNAME}"
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" 
    PATH_SUFFIXES "build/${LIBNAME}" "../build/${LIBNAME}" "${LIBNAME}" "../${LIBNAME}" "${LIBNAME}/build" "../${LIBNAME}/build"
  )
  if ( ${${LIBBIN}} )
    link_directories("${${LIBBIN}}")
    MESSAGE(STATUS "${LIBBIN} = ${${LIBBIN}}")
  endif()
ENDMACRO(get_mambaru)

