enable_testing()

#INCLUDE(FindThreads)

find_package( Threads )
find_package( Boost COMPONENTS system )

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

IF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")
    exec_program(
      ${CMAKE_CXX_COMPILER}
      ARGS -dumpversion
      OUTPUT_VARIABLE gcc_compiler_version
    )        

    STRING(REGEX REPLACE "^([0-9]+)\\..*" "\\1" gcc_major_version "${gcc_compiler_version}")
    STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" gcc_minor_version "${gcc_compiler_version}")
    MATH(EXPR gcc_version_number "${gcc_major_version} * 1000 + ${gcc_minor_version}" )

    message(STATUS "C++ compiler version: ${gcc_compiler_version} major: ${gcc_major_version} minor: ${gcc_minor_version} number: ${gcc_version_number} [${CMAKE_CXX_COMPILER}]")

    SET(CMAKE_CXX_STANDARD    "-std=c++0x")
    if ( NOT ${gcc_version_number} LESS 4008 )
      SET(CMAKE_CXX_STANDARD    "${CMAKE_CXX_STANDARD} -ftemplate-backtrace-limit=0")
    endif()
    SET(CMAKE_CXX_WARN_FLAGS  "-W -Wall -pedantic -Wextra")
    SET(CMAKE_CXX_DEFINES     "-D_THREAD_SAFE -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DBOOST_SYSTEM_NO_DEPRECATED -D_GLIBCXX_USE_NANOSLEEP")
    SET(CMAKE_CXX_ARCH_FLAGS  "-mfpmath=sse -msse2")
    SET(CMAKE_CXX_FLAGS       "${CXX_FLAGS} ${CMAKE_CXX_STANDARD} ${CMAKE_CXX_WARN_FLAGS} ${CMAKE_CXX_DEFINES} ${CMAKE_CXX_ARCH_FLAGS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb ")
    SET(CMAKE_C_FLAGS_RELEASE "-O2 -ggdb -march=core2 -DNDEBUG")
ENDIF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")

SET(FASLIB_DIR "${CMAKE_SOURCE_DIR}/../../faslib")
SET(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")
set(IOW_TEST_DIR "${CMAKE_BINARY_DIR}/bin-test")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#include_directories(..)
#include_directories(../faslib/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})

add_subdirectory(owner)
add_subdirectory(aux)
add_subdirectory(io)
add_subdirectory(jsonrpc)
