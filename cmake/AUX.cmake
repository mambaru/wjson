if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

get_property(cur_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

include(ConfigureLibrary)
CONFIGURE_LIBRARY( fas/aop.hpp "${cur_dirs} \
                                ${CMAKE_CURRENT_SOURCE_DIR}/../faslib \
                                ${CMAKE_SOURCE_DIR}/../faslib \
                                ${PROJECT_BINARY_DIR}/faslib \
                                /usr/include/faslib\
                                /usr/local/include/faslib" 
                  faslib "" )
if ( NOT HAVE_INCLUDE_faslib )
  message(WARNING "faslib not found! Use 'git clone https://github.com/migashko/faslib.git' \
                   in parent directory or install to system and set path in you project")
endif()


if ( ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") 
      OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") )
      
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror -pedantic -fno-rtti")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
  
  if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")
  endif()


  if ( PARANOID_WARNING )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -ftemplate-depth=1400 -Wcast-align -Winvalid-pch -pedantic-errors  -Wformat-nonliteral")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcomment -Wconversion -Wformat-security -Wimport  -Wchar-subscripts")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat-y2k")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlong-long -Wmissing-braces -Wmissing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type -Wsequence-point -Wsign-compare -Wstack-protector -Wuninitialized")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch-enum -Wtrigraphs -Wunknown-pragmas")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal  -Wformat -Wformat=2 -Winit-self  -Wmissing-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable  -Wvariadic-macros -Wvolatile-register-var  -Wwrite-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-overflow=2 -Wswitch -Wswitch-default -Wundef -Werror")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      # -Wunsafe-loop-optimizations
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op  -Wnoexcept -Wstrict-null-sentinel")
    endif()
  endif(PARANOID_WARNING)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /W3 /wd4503 /wd4307 /EHsc /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG  "/Od /D_DEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "/Yd /O2 /DNDEBUG")
endif()

if ( NOT FASLIB_DIR )
  if ( HAVE_INCLUDE_faslib )
    set(FASLIB_DIR "${HAVE_INCLUDE_faslib}")
  elseif ( BUILD_TESTING )
    execute_process(COMMAND git clone https://github.com/migashko/faslib.git WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
    execute_process(COMMAND mkdir -p build WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/faslib")
    execute_process(COMMAND cmake .. WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/faslib/build")
    set(FASLIB_DIR "${PROJECT_BINARY_DIR}/faslib")
  endif()
endif( NOT FASLIB_DIR )

set(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")

include_directories(${FASLIB_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
