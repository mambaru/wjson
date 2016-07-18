enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IOW_TEST_DIR})

set(
  CMAKE_CXX_FLAGS 
"-std=c++11 -pedantic -Wall -Wextra -Wcast-align \
-Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization \
-Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
-Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
-Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused"
)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../faslib)

add_executable(example1 example1.cpp)
add_executable(example2 example2.cpp)
add_executable(example3 example3.cpp)
add_executable(example4 example4.cpp)
add_executable(example5 example5.cpp)
