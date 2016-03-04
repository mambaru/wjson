cmake_minimum_required(VERSION 2.6)

project(iow)

include_directories(.)

# TODO:
#SET(CMAKE_CXX_STANDARD    "-std=c++0x")
#SET(CMAKE_CXX_FLAGS       "${CXX_FLAGS} ${CMAKE_CXX_STANDARD} ")
#SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ${CMAKE_CXX_FLAGS}")
#SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")
#SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")

add_library(
  iow
  iow/logger/global_log.cpp
  iow/jsonrpc/incoming/incoming_holder.cpp
  iow/jsonrpc/incoming/aux.cpp
  iow/thread/delayed_queue.cpp
  #iow/jsonrpc/engine/io_registry.cpp
)

target_link_libraries(iow ${Boost_LIBRARIES} )

if ( IOW_BUILD_TEST OR IOW_BUILD_ALL )
  if( IOW_BUILD_TEST )
    enable_testing()
    add_subdirectory(tests)
  endif() # IOW_BUILD_TEST
endif()


#add_subdirectory(tests/jsonrpc)