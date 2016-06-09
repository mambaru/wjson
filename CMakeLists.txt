cmake_minimum_required(VERSION 2.6)

project(iow)

include_directories(.)

add_library(
  iow
  iow/logger/global_log.cpp
  iow/workflow/workflow.cpp
  iow/workflow/delayed_queue.cpp
  iow/workflow/asio_queue.cpp
  iow/workflow/bique.cpp
  iow/jsonrpc/incoming/incoming_holder.cpp
  iow/jsonrpc/outgoing/outgoing_holder.cpp
  iow/jsonrpc/incoming/aux.cpp
)

target_link_libraries(iow ${Boost_LIBRARIES} )

if ( IOW_BUILD_TEST OR IOW_BUILD_ALL )
  if( IOW_BUILD_TEST )
    enable_testing()
    add_subdirectory(tests)
  endif() # IOW_BUILD_TEST
endif()

