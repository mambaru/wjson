enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})

add_subdirectory(owner)
add_subdirectory(aux)
add_subdirectory(io)
add_subdirectory(thread)
add_subdirectory(jsonrpc)
