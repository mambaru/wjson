set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})

link_libraries(faslib wjson)

add_executable(example1 example1.cpp)
add_executable(example2 example2.cpp)
add_executable(example3 example3.cpp)
add_executable(example4 example4.cpp)
add_executable(example5 example5.cpp)
add_executable(example6 example6.cpp)
add_executable(example7 example7.cpp)
add_executable(example8 example8.cpp)
add_executable(example10 example10.cpp)
add_executable(example11 example11.cpp)
add_executable(example13 example13.cpp)
add_executable(example14 example14.cpp)
add_executable(example15 example15.cpp)
add_executable(example16 example16.cpp)
add_executable(example17 example17.cpp)

wci_utils(example1 example2 example3 example4 example5)
wci_utils(example6 example7 example8 example10 example11)
wci_utils(example13 example14 example15 example16 example17)

if ( NOT ${CMAKE_CXX_STANDARD} EQUAL 98 )
  add_executable(example9 example9.cpp)
  add_executable(example12 example12.cpp)
  wci_utils(example9 example12)
endif()


