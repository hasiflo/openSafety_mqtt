#### PySODBuilder

# Generates the source files based on a given osdd file
MACRO ( PySODBuilder SOURCE_FILE_PATH OUTPUT_PATH
        OSDD_PATH SETTINGS_FILE MODULE_ID DEPENDS_ON WORKING_DIR )

    IF ( NOT EXISTS ${OSDD_PATH} )
        MESSAGE ( FATAL_ERROR "${PROJECT_NAME}: The provided OSDD path \
            [${OSDD_PATH}] does not exist!" )
        RETURN ()
    ENDIF ( NOT EXISTS ${OSDD_PATH} )

    IF ( NOT EXISTS ${SOURCE_FILE_PATH} )
        MESSAGE ( FATAL_ERROR "${PROJECT_NAME}: The provided \
            PYSODBUILDER_INPUT_DIR [${SOURCE_FILE_PATH}] does not exist!" )
        RETURN ()
    ENDIF ( NOT EXISTS ${SOURCE_FILE_PATH} )

    GET_FILENAME_COMPONENT ( _osddPath ${OSDD_PATH} NAME )

    SET ( PYSODBUILDER_SCRIPT "pysodbuilder.py" )

    IF ( NOT "${MODULE_ID}" STREQUAL "" )
        SET ( PYSODBUILDER_MODULE_ARGUMENT "-m ${MODULE_ID}" )
    ENDIF ()

    ADD_CUSTOM_TARGET ( pysodbuilder ALL
                COMMAND ${PYTHON_EXECUTABLE} -m pysodb.pysodbmain
                -i ${SOURCE_FILE_PATH}
                -o ${OUTPUT_PATH}
                -d ${OSDD_PATH} ${PYSODBUILDER_MODULE_ARGUMENT}
                -s ${SETTINGS_FILE}
                --remove-cog-comments
                COMMENT "Generating source files based on ${_osddPath}"
                DEPENDS ${DEPENDS_ON}
                WORKING_DIRECTORY ${WORKING_DIR}
                VERBATIM
    )

ENDMACRO ( PySODBuilder SOURCE_FILE_PATH OUTPUT_PATH
        OSDD_PATH MODULE_ID DEPENDS_ON WORKING_DIR )

# Copies the pysodbuilder tool to the given destination
MACRO ( PySODBuilderCopy SOURCE_DIR DESTINATION_DIR TIMESTAMP_FILE DEPENDS_ON )

    # custom command for build time copying
    ADD_CUSTOM_COMMAND ( OUTPUT ${TIMESTAMP_FILE}
                        COMMAND ${CMAKE_COMMAND} -E touch
                        ${TIMESTAMP_FILE}
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${SOURCE_DIR} ${DESTINATION_DIR}
                        COMMENT "Copying pysodbuider sources to build directory"
                        DEPENDS ${DEPENDS_ON}
    )

    # configuration time copying (only new / changed files)
    FILE ( COPY ${SOURCE_DIR}/ DESTINATION ${DESTINATION_DIR} )

    EXECUTE_PROCESS(
        COMMAND ${CMAKE_COMMAND} -E touch ${TIMESTAMP_FILE}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

ENDMACRO ( PySODBuilderCopy SOURCE_DIR DESTINATION_DIR
        TIMESTAMP_FILE DEPENDS_ON )

MACRO ( PySODBuilderCheckPythonModules MODULES )

    # try to locate needed third party python packages / modules
    FOREACH (_module IN ITEMS ${MODULES} )
        STRING ( TOUPPER ${_module} module_upper )
        LOCATE_PYTHON_MODULE ( ${_module} )
        IF ( NOT PY_${module_upper} )
            MESSAGE ( WARNING
            "${PROJECT_NAME}: Python module ${_module} not found! Please install" )
        ENDIF()
    ENDFOREACH ()

ENDMACRO ( PySODBuilderCheckPythonModules MODULES )

# Genereates the binding file based on a given OSDD schema file
MACRO ( PySODBuilderGenerateBinding BINDING_FILE SCHEMA_FILE
        DEPENDS_ON WORKING_DIR )

    IF ( NOT EXISTS ${SCHEMA_FILE} )
        MESSAGE ( FATAL_ERROR "${PROJECT_NAME}: File \
            ${SCHEMA_FILE} does not exist!" )
    ENDIF()

    ADD_CUSTOM_COMMAND (
        OUTPUT ${BINDING_FILE}
        COMMAND ${PYTHON_EXECUTABLE} -m pysodb.pysodbmain
        --generate-binding-file ${SCHEMA_FILE}
        ${BINDING_FILE}
        COMMENT "Generating pysodbuilder binding file"
        WORKING_DIRECTORY ${WORKING_DIR}
        DEPENDS ${DEPENDS_ON}
        VERBATIM
    )

    # generate new binding file, if schema file is newer than binding file or
    # binding does not exist
    IF ( ${SCHEMA_FILE} IS_NEWER_THAN ${BINDING_FILE} )
        EXECUTE_PROCESS (
            COMMAND ${PYTHON_EXECUTABLE} -m pysodb.pysodbmain
            --generate-binding-file ${SCHEMA_FILE} ${BINDING_FILE}
            WORKING_DIRECTORY ${WORKING_DIR}
        )
ENDIF()

ENDMACRO ( PySODBuilderGenerateBinding BINDING_FILE SCHEMA_FILE
        DEPENDS_ON WORKING_DIR )

# Sets cache variables to demo files, if they have not been set and the variable
# DEMO_ENABLE is true
MACRO ( PySODBuilder_Set_Demo_Paths )

    IF ( BUILD_OPENSAFETY_PYTHON_TOOLS )
        IF ( DEFINED PYSODBUILDER_INPUT_DIR AND NOT PYSODBUILDER_INPUT_DIR )
            SET ( PYSODBUILDER_INPUT_DIR
                "${PROJECT_SOURCE_DIR}/demo_files/input"
                CACHE PATH "directory of input source files" FORCE )
        ENDIF()

        IF ( DEFINED PYSODBUILDER_OSDD_FILE AND NOT PYSODBUILDER_OSDD_FILE )
            SET ( PYSODBUILDER_OSDD_FILE
                "${PROJECT_SOURCE_DIR}/demo_files/osdd/demo.xosdd"
                CACHE FILEPATH "osdd file to use for pysodbuilder" FORCE )
        ENDIF()

        IF ( DEFINED PYSODBUILDER_SETTINGS_FILE AND
            NOT PYSODBUILDER_SETTINGS_FILE )

            SET ( PYSODBUILDER_SETTINGS_FILE
                "${PROJECT_BINARY_DIR}/pysodbsettings.ini" CACHE FILEPATH
                "settings file to use for pysodbuilder" FORCE )
        ENDIF()

        IF ( DEFINED PYSODBUILDER_OUTPUT_DIR AND NOT PYSODBUILDER_OUTPUT_DIR )
            SET ( PYSODBUILDER_OUTPUT_DIR
                "${PROJECT_BINARY_DIR}/demo_files/output"
                CACHE PATH "direcotry of the outputfiles" FORCE )
        ENDIF()

        IF ( DEFINED PYSODBUILDER_OSDD_SCHEMA_FILE AND
            NOT PYSODBUILDER_OSDD_SCHEMA_FILE )

            IF ( NOT EXISTS "${CMAKE_SOURCE_DIR}/../doc/XOSDD_R15.xsd" )
                MESSAGE ( FATAL_ERROR "Default OSDD XSD file not found!" )
            ELSE()
                SET ( PYSODBUILDER_OSDD_SCHEMA_FILE
                    "${CMAKE_SOURCE_DIR}/../doc/XOSDD_R15.xsd" CACHE FILEPATH
                    "OSDD schema file" FORCE )
            ENDIF()
        ENDIF()
    ENDIF( BUILD_OPENSAFETY_PYTHON_TOOLS )

ENDMACRO ( PySODBuilder_Set_Demo_Paths )
