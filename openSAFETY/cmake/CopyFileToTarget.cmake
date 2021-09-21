
### Macro for copying a file to the target build directory
MACRO(CopyFileToTarget TARGET_NAME FILE_LOCATION)

    IF(NOT EXISTS ${FILE_LOCATION})
        MESSAGE(FATAL_ERROR "Unable to copy file '${FILE_LOCATION}'! File is not existing!")
    ENDIF()

    GET_FILENAME_COMPONENT(FILE_NAME ${FILE_LOCATION} NAME)

    ADD_CUSTOM_COMMAND ( TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE_LOCATION}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
        COMMENT "Copy file ${FILE_NAME} to ${TARGET_NAME} build ..."
    )

    IF(MSVC)
        # For VisualStudio during debugging the file also needs to be copied to the build folder without Release/Debug subfolder
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE_LOCATION}" "${PROJECT_BINARY_DIR}"
                        RESULT_VARIABLE COPY_RES
                        ERROR_VARIABLE COPY_STDERR
        )

        IF( NOT ${COPY_RES} MATCHES "0" )
            MESSAGE ( FATAL_ERROR "${CMAKE_COMMAND} returned error: ${COPY_STDERR}" )
        ENDIF()
    ENDIF()

ENDMACRO(CopyFileToTarget)
