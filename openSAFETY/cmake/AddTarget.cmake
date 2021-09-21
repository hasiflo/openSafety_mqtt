#### AddTarget

# FILE ( MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/targets")

# Instructions for adding a simple test
MACRO(AddTarget OPTION_NAME SOURCE_DIR TARGET_DESCRIPTION TARGET_ENABLED)
    STRING ( TOUPPER "TARGET_${OPTION_NAME}" STR_OPTION_NAME )
    STRING ( CONFIGURE "${STR_OPTION_NAME}" OPTION_VALUE )
    OPTION ( ${STR_OPTION_NAME} ${TARGET_DESCRIPTION} ${TARGET_ENABLED} )

    IF ( ${OPTION_VALUE} )
        INCLUDE_DIRECTORIES ( ${SOURCE_DIR} )
        ADD_SUBDIRECTORY ( ${SOURCE_DIR} )

        GET_TARGET_PROPERTY(targetLocation "${OPTION_NAME}" LOCATION)

        IF ( "${targetLocation}" STREQUAL "${OPTION_NAME}-NOTFOUND" )
            MESSAGE ( FATAL_ERROR "AddTarget : The given target name ${OPTION_NAME} is not a valid target " )
        ELSE ( "${targetLocation}" STREQUAL "${OPTION_NAME}-NOTFOUND" )
            MESSAGE ( STATUS "  TARGET ${OPTION_NAME} " )
        ENDIF ( "${targetLocation}" STREQUAL "${OPTION_NAME}-NOTFOUND" )

        ADD_DEPENDENCIES ( ${OPTION_NAME} "osdd2sod" )

    ENDIF ( ${OPTION_VALUE} )

ENDMACRO(AddTarget)

SET ( TARGET_GENERIC FALSE )
SET ( TARGET_ARM_ELF FALSE )
SET ( TARGET_COLDFIRE FALSE )
SET ( TARGET_WINDOWS FALSE )

IF ( WIN32 )
    SET ( TARGET_WINDOWS TRUE )
ELSE ( WIN32 )

    SET ( EXECUTE_RESULT "" )
    EXECUTE_PROCESS (
        COMMAND ${CMAKE_C_COMPILER} -v
        ERROR_VARIABLE EXECUTE_RESULT
    )

    SET ( TARGET_VAR "" )

    EXECUTE_PROCESS (
        COMMAND echo ${EXECUTE_RESULT}
        COMMAND grep "Target:"
        COMMAND cut -c 9-
        OUTPUT_VARIABLE TARGET_VAR
    )

    IF ( TARGET_VAR )
        STRING( STRIP ${TARGET_VAR} TARGET_VAR )

        STRING ( COMPARE EQUAL ${TARGET_VAR} "arm-elf" TARGET_ARM_ELF )
        STRING ( COMPARE EQUAL ${TARGET_VAR} "m68k-elf" TARGET_COLDFIRE )
    ENDIF ( TARGET_VAR )

ENDIF ( WIN32 )

IF ( NOT TARGET_ARM_ELF AND NOT TARGET_COLDFIRE )
    SET ( TARGET_VAR "Generic" )
    SET ( TARGET_GENERIC TRUE )
ENDIF ( NOT TARGET_ARM_ELF AND NOT TARGET_COLDFIRE )

MESSAGE ( STATUS "Found compiler for target architecture: ${TARGET_VAR}" )