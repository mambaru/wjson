set(VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}")

message(STATUS "${PROJECT_VERSION}: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if ( IS_DIRECTORY ${PROJECT_NAME} )
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/version.hpp.in"
                "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/version.hpp" )
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/version.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/version.yml" )


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(package_config_destination ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/../config/projectConfig.cmake.in ${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${package_config_destination}
)
write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION ${package_config_destination}
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${package_config_destination}
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
  ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

if ( IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${PROJECT_NAME} )
  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/"
    PATTERN *.hpp
  )
endif()

