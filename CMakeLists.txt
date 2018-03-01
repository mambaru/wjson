cmake_minimum_required(VERSION 3.5)

project(wjson)

if ( BUILD_TESTING )
  if (PARANOID_WARNING)
    if(UNIX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align \
                         -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization\
                         -Wformat=2 -Winit-self -Wmissing-declarations \
                         -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual \
                         -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
                         -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror")
      # -Wlogical-op -Wnoexcept -Wstrict-null-sentinel
    endif()
  endif()
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

