cmake_minimum_required(VERSION 2.6)

project(wjson)

if ( BUILD_TESTING )
    if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align \
                         -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization\
                         -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
                         -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
                         -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
                         -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror")
    endif()

  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

