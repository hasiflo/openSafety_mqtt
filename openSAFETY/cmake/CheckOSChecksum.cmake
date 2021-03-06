

MACRO(COPY_CHECK_FILE_ONLY ORIG_FILE CHECK_FILE )
    IF ( ${ORIG_FILE} IS_NEWER_THAN ${CHECK_FILE} )
        EXECUTE_PROCESS (
            COMMAND ${CMAKE_COMMAND} -E remove -f "${CHECK_FILE}"
            OUTPUT_QUIET)
    ENDIF ( ${ORIG_FILE} IS_NEWER_THAN ${CHECK_FILE} )

    IF ( NOT EXISTS ${CHECK_FILE} )
        EXECUTE_PROCESS (
            COMMAND ${CMAKE_COMMAND} -E copy "${ORIG_FILE}" "${CHECK_FILE}"
            OUTPUT_QUIET)
    ENDIF ( NOT EXISTS ${CHECK_FILE} )
ENDMACRO()

MACRO(COPY_WITH_RUNNER ORIG_FILE CHECK_FILE TARGET_NAME)

    SET ( SCRIPT_NAME )
    IF ( PYTHON_EXECUTABLE )
        SET ( SCRIPT_NAME "${CMAKE_SOURCE_DIR}/../contrib/scripts/rmcomments.py" )
    ENDIF()

    IF ( EXISTS ${ORIG_FILE} )
        GET_FILENAME_COMPONENT(NEWPATH ${CHECK_FILE} PATH )
        IF ( NOT IS_DIRECTORY ${NEWPATH} )
            EXECUTE_PROCESS (
                COMMAND ${CMAKE_COMMAND} -E make_directory "${NEWPATH}"
                OUTPUT_QUIET)
        ENDIF ( NOT IS_DIRECTORY ${NEWPATH} )
    ENDIF ( EXISTS ${ORIG_FILE} )

    ADD_CUSTOM_COMMAND (
        TARGET "${TARGET_NAME}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${NEWPATH}"
        COMMAND ${PYTHON_EXECUTABLE} ${SCRIPT_NAME} ${ORIG_FILE} ${CHECK_FILE}
        DEPENDS ${ORIG_FILE} "oschecksum" "oscrccheck"
    )
ENDMACRO()

MACRO(AddCRCCheck LIST_FILE CHECK_TARGET CHECK_NAME ORIG_FILE TARGET_PATH )
    SET ( COLOR )
    IF ( CMAKE_COLOR_MAKEFILE )
        SET ( COLOR "-x" )
    ENDIF ( CMAKE_COLOR_MAKEFILE )

    SET ( WARN_CRC_CHECK_FAIL "OFF" )
    IF ( CHECKSUM_SRC_WARN_ONLY_ON_FAIL )
        SET ( WARN_CRC_CHECK_FAIL "ON" )
    ENDIF ( CHECKSUM_SRC_WARN_ONLY_ON_FAIL )

    SET ( TARGET_NAME "CRC_CHECK_${CHECK_TARGET}" )

    IF ( NOT TARGET "${TARGET_NAME}" )
        ADD_CUSTOM_TARGET ( "${TARGET_NAME}_PREP" ALL
            COMMENT "Preparing CRC checks for [${CHECK_TARGET}]" )
        ADD_CUSTOM_TARGET ( "${TARGET_NAME}" ALL
            COMMENT "Checking CRC checksum for [${CHECK_NAME}]" )

        ADD_DEPENDENCIES ( ${TARGET_NAME} "${TARGET_NAME}_PREP" )
    ENDIF()

    SET(oschecksumLocation  $<TARGET_FILE:oscrccheck>)

    GET_FILENAME_COMPONENT( FILE_PATH ${ORIG_FILE} NAME )

    SET ( CHECK_FILE "${TARGET_PATH}/${FILE_PATH}" )

    IF ( PYTHON_EXECUTABLE )
        COPY_WITH_RUNNER(${ORIG_FILE} ${CHECK_FILE} "${TARGET_NAME}_PREP" )
    ELSE ()
        COPY_CHECK_FILE_ONLY( ${ORIG_FILE} ${CHECK_FILE})
    ENDIF ()

    ADD_CUSTOM_COMMAND (
        TARGET ${TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -D CRC_LOCATION=${oschecksumLocation} -D COLOR=${COLOR} -D LIST=${LIST_FILE}
            -D FILE=${CHECK_FILE} -D CRC_CHECK=${WARN_CRC_CHECK_FAIL} -P "${CMAKE_SOURCE_DIR}/../cmake/CheckOSChecksumRunner.cmake"
        COMMENT "\tChecking CRC checksum for [${CHECK_NAME}] ${FILE_PATH}"
        DEPENDS ${CHECK_FILE}
        VERBATIM
    )
ENDMACRO()

MACRO(CheckCRCCode STACK_PART LIST_FILE SOURCE_FILE CHECKER )
    IF ( CHECKSUM_SRC_SOURCECODE )
        FOREACH(_currentItem ${SOURCE_FILE})
            AddCRCCheck ( "${LIST_FILE}" "${STACK_PART}" "${STACK_PART}" "${_currentItem}" "${CMAKE_BINARY_DIR}/crc/eplssrc/${STACK_PART}" )
        ENDFOREACH(_currentItem)
    ENDIF ( CHECKSUM_SRC_SOURCECODE )

    ADD_DEPENDENCIES ( CRC_CHECK_STACK "CRC_CHECK_${STACK_PART}" )

ENDMACRO(CheckCRCCode)

ADD_CUSTOM_TARGET ( "CRC_CHECK_STACK" ALL
    COMMENT "Checking openSAFETY Stack crc"
)

IF ( TARGET oscrccheck_marker )
    ADD_DEPENDENCIES ( CRC_CHECK_STACK oscrccheck oscrccheck_marker )
ENDIF()

SET ( openSAFETY_STACK_FILE "${oscrccheck_SOURCE_DIR}/oschecksum.crc" )
SET ( openSAFETY_STACK_CRC "3527856b" )

SET(oschecksumLocation  $<TARGET_FILE:oscrccheck>)

SET ( WARN_CRC_CHECK_FAIL "OFF" )
IF ( CHECK_CRC_WARN_ONLY_ON_FAIL )
    SET ( WARN_CRC_CHECK_FAIL "ON" )
ENDIF ( CHECK_CRC_WARN_ONLY_ON_FAIL )

ADD_CUSTOM_COMMAND (
    TARGET CRC_CHECK_STACK
    COMMAND ${CMAKE_COMMAND} -D CRC_LOCATION=${oschecksumLocation}
        -D openSAFETY_STACK_CRC=${openSAFETY_STACK_CRC} -D openSAFETY_STACK_FILE=${openSAFETY_STACK_FILE}
        -D CRC_CHECK=${WARN_CRC_CHECK_FAIL} -P "${CMAKE_SOURCE_DIR}/../cmake/CheckOSChecksumStackRunner.cmake"
    COMMENT "\tChecking if openSAFETY stack crc is valid"
    VERBATIM
)
