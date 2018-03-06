# ConfigureLibrary.cmake
#
# Configure library
#
MACRO(CONFIGURE_LIBRARY HEADER_NAME HEADER_PATHS LIBRARY_NAME LIBRAY_PATHS)
    # Search headers
    STRING(REGEX MATCHALL "([^ ]+)" S_HEADER_PATHS ${HEADER_PATHS})
    FOREACH(TMP_INCLUDE ${S_HEADER_PATHS})
        FIND_PATH(HAVE_INCLUDE_${LIBRARY_NAME} ${HEADER_NAME}
                  ${TMP_INCLUDE}
                  DOC "Path where the ${LIBRARY_NAME} header files (${HEADER_NAME}) can be found")

        IF(HAVE_INCLUDE_${LIBRARY_NAME})
            BREAK()
        ENDIF(HAVE_INCLUDE_${LIBRARY_NAME})
    ENDFOREACH(TMP_INCLUDE)

    # Header file found
    IF(HAVE_INCLUDE_${LIBRARY_NAME})
      if ( LIBRAY_PATHS )
        MESSAGE(STATUS "Header file ${HEADER_NAME} found in: ${HAVE_INCLUDE_${LIBRARY_NAME}}")
        INCLUDE_DIRECTORIES(${HAVE_INCLUDE_${LIBRARY_NAME}})

        STRING(REGEX MATCHALL "([^ ]+)" S_LIBRARY_PATHS ${LIBRAY_PATHS})
        FOREACH(TMP_LIBRARY ${S_LIBRARY_PATHS})
            FIND_LIBRARY(HAVE_${LIBRARY_NAME}
                         NAMES ${LIBRARY_NAME}
                         PATHS ${TMP_LIBRARY}
                         DOC "Path where the ${LIBRARY_NAME} libraries can be found")
            IF (HAVE_${LIBRARY_NAME})
                MESSAGE(STATUS "Library ${LIBRARY_NAME} found here: ${HAVE_${LIBRARY_NAME}}")
                BREAK()
            ENDIF (HAVE_${LIBRARY_NAME})
        ENDFOREACH(TMP_LIBRARY)
      endif()
    ELSE (NOT HAVE_INCLUDE_${LIBRARY_NAME})
        MESSAGE(STATUS "#### Cannot find ${LIBRARY_NAME} header file ${HEADER_NAME}")
    ENDIF(HAVE_INCLUDE_${LIBRARY_NAME})

ENDMACRO(CONFIGURE_LIBRARY)
#
