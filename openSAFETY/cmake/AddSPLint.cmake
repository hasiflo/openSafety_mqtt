
INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM(SPLINT_EXECUTABLE splint HINTS ${GCOV_ROOT} "${GCOV_ROOT}/bin" )

FIND_PACKAGE_HANDLE_STANDARD_ARGS(splint DEFAULT_MSG SPLINT_EXECUTABLE)

# only visible in advanced view
MARK_AS_ADVANCED(SPLINT_EXECUTABLE)


FUNCTION(add_splint TARGET)
    if(${SPLINT_FOUND})

        SET ( SPLINT_FLAGS +ignorequals -nullpass -nullstate -unreachable +quiet +showsummary )

        IF ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
            SET ( SPLINT_FLAGS +posixlib ${SPLINT_FLAGS} )
        ENDIF()
        IF ( WIN32 )
            SET ( SPLINT_FLAGS ${SPLINT_FLAGS}
                -linelen 999
                -I.
                -D_WIN32
                -D_NTSYSTEM
                -DNOGDI
                -DNOKERNEL
                -DNOUSER
            )
        ENDIF ()

        get_directory_property(include_dirs INCLUDE_DIRECTORIES)

        foreach(incdir ${include_dirs})
            list(APPEND include_flags -I${incdir})
        endforeach()

        add_custom_target(
            ${TARGET}_splint
            COMMAND ${SPLINT_EXECUTABLE} ${SPLINT_FLAGS} ${include_flags} ${ARGN}
            DEPENDS ${ARGN}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            )
        #add_dependencies(${TARGET} ${TARGET}_splint)
    endif()
ENDFUNCTION()
