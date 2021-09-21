# - Creates a special coverage build type and target on GCC.
#
# Defines a function ENABLE_COVERAGE_REPORT which generates the coverage target
# for selected targets. Optional arguments to this function are used to filter
# unwanted results using globbing expressions. Moreover targets with tests for
# the source code can be specified to trigger regenerating the report if the
# test has changed
#
# ENABLE_COVERAGE_REPORT(TARGETS target... [FILTER filter...] [TESTS test targets...])
#
# To generate a coverage report first build the project with
# CMAKE_BUILD_TYPE=coverage, then call make test and afterwards make coverage.
#
# The coverage report is based on gcov. Depending on the availability of lcov
# a HTML report will be generated and/or an XML report of gcovr is found.
# The generated coverage target executes all found solutions. Special targets
# exist to create e.g. only the xml report: coverage-xml.
#
# Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
#
# This file may be licensed under the terms of the
# GNU Lesser General Public License Version 3 (the ``LGPL''),
# or (at your option) any later version.
#
# Software distributed under the License is distributed
# on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
# express or implied. See the LGPL for the specific language
# governing rights and limitations.
#
# You should have received a copy of the LGPL along with this
# program. If not, go to http://www.gnu.org/licenses/lgpl.html
# or write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The development of this software was supported by:
#   CoR-Lab, Research Institute for Cognition and Robotics
#     Bielefeld University

# taken from http://www.cmake.org/Wiki/CMakeMacroParseArguments

MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
    SET(DEFAULT_ARGS)
    FOREACH(arg_name ${arg_names})
        SET(${prefix}_${arg_name})
    ENDFOREACH(arg_name)
    FOREACH(option ${option_names})
        SET(${prefix}_${option} FALSE)
    ENDFOREACH(option)

    SET(current_arg_name DEFAULT_ARGS)
    SET(current_arg_list)
    FOREACH(arg ${ARGN})
        SET(larg_names ${arg_names})
        LIST(FIND larg_names "${arg}" is_arg_name)
        IF (is_arg_name GREATER -1)
            SET(${prefix}_${current_arg_name} ${current_arg_list})
            SET(current_arg_name ${arg})
            SET(current_arg_list)
        ELSE (is_arg_name GREATER -1)
            SET(loption_names ${option_names})
            LIST(FIND loption_names "${arg}" is_option)
            IF (is_option GREATER -1)
                SET(${prefix}_${arg} TRUE)
            ELSE (is_option GREATER -1)
                SET(current_arg_list ${current_arg_list} ${arg})
            ENDIF (is_option GREATER -1)
        ENDIF (is_arg_name GREATER -1)
    ENDFOREACH(arg)
    SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

FUNCTION(ENABLE_COVERAGE_REPORT)

    # argument parsing
    PARSE_ARGUMENTS(ARG "TARGETS;COVERAGE;BINARY_DIR;FILTER" "" ${ARGN})

    SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} coverage PARENT_SCOPE)

    # decide if there is any tool to create coverage data
    IF(NOT GCOVR_FOUND)
        MESSAGE(STATUS "Cannot enable coverage targets because gcovr is not found.")
    ENDIF()

    # decide if there is any tool to create coverage data
    SET(TOOL_FOUND FALSE)
    IF(LCOV_FOUND OR GCOVR_FOUND)
        SET(TOOL_FOUND TRUE)
    ENDIF()
    IF(NOT TOOL_FOUND)
        MESSAGE(STATUS "Cannot enable coverage targets because neither lcov nor gcovr are found.")
    ENDIF()

    STRING(TOLOWER "${CMAKE_BUILD_TYPE}" COVERAGE_BUILD_TYPE)
    IF(CMAKE_COMPILER_IS_GNUCXX AND TOOL_FOUND AND "${COVERAGE_BUILD_TYPE}" MATCHES "coverage")
        MESSAGE ( STATUS "Enabling coverage report for [${ARG_COVERAGE}]" )

        # create coverage build type
        SET(CMAKE_CXX_FLAGS_COVERAGE ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
        SET(CMAKE_C_FLAGS_COVERAGE ${CMAKE_C_FLAGS_DEBUG} PARENT_SCOPE)
        SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} coverage PARENT_SCOPE)

        FOREACH ( ARG_TARGET IN ITEMS ${ARG_TARGETS} )

            # instrument targets
            GET_TARGET_PROPERTY(TARGET_COMPILE_FLAGS ${ARG_TARGET} COMPILE_FLAGS )
            IF ( "${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND" )
                SET ( TARGET_COMPILE_FLAGS "" )
            ENDIF ( "${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND" )
            SET ( TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS} --coverage -g -pg " )

            GET_TARGET_PROPERTY(TARGET_LINK_FLAGS ${ARG_TARGET} LINK_FLAGS )
            IF ( "${TARGET_LINK_FLAGS}" STREQUAL "TARGET_LINK_FLAGS-NOTFOUND" )
                SET ( TARGET_LINK_FLAGS "" )
            ENDIF ( "${TARGET_LINK_FLAGS}" STREQUAL "TARGET_LINK_FLAGS-NOTFOUND" )
            SET ( TARGET_LINK_FLAGS "${TARGET_LINK_FLAGS} --coverage -g -pg " )

            SET_TARGET_PROPERTIES(${ARG_TARGET} PROPERTIES COMPILE_FLAGS ${TARGET_COMPILE_FLAGS}
                                                            LINK_FLAGS ${TARGET_LINK_FLAGS} )

        ENDFOREACH ( ARG_TARGET IN ITEMS ${ARG_TARGETS} )

        # html report
        IF (LCOV_FOUND)

            MESSAGE(STATUS "Enabling HTML coverage report")

            SET(COVERAGE_RAW_FILE "${CMAKE_BINARY_DIR}/${ARG_COVERAGE}_coverage.raw.info")
            SET(COVERAGE_FILTERED_FILE "${CMAKE_BINARY_DIR}/${ARG_COVERAGE}_coverage.info")
            SET(COVERAGE_REPORT_DIR "${CMAKE_BINARY_DIR}/${ARG_COVERAGE}_coveragereport")

            # set up coverage target

            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_RAW_FILE}
                               COMMAND ${LCOV_EXECUTABLE} -c -d ${CMAKE_BINARY_DIR} -b "${CMAKE_SOURCE_DIR}/" -o ${COVERAGE_RAW_FILE}
                               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                               COMMENT "Collecting coverage data"
                               DEPENDS ${ARG_TARGETS}
                               VERBATIM)

            # filter unwanted stuff
            LIST(LENGTH ARG_FILTER FILTER_LENGTH)
            IF(${FILTER_LENGTH} GREATER 0)
                SET(FILTER COMMAND ${LCOV_EXECUTABLE})
                FOREACH(F ${ARG_FILTER})
                    SET(FILTER ${FILTER} -r ${COVERAGE_FILTERED_FILE} ${F})
                ENDFOREACH()
                SET(FILTER ${FILTER} -o ${COVERAGE_FILTERED_FILE})
            ELSE()
                SET(FILTER "")
            ENDIF()

            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_FILTERED_FILE}
                               COMMAND ${LCOV_EXECUTABLE} -e ${COVERAGE_RAW_FILE} -b "${CMAKE_SOURCE_DIR}/" "${CMAKE_SOURCE_DIR}*"  -o ${COVERAGE_FILTERED_FILE}
                               ${FILTER}
                               DEPENDS ${COVERAGE_RAW_FILE}
                               COMMENT "Filtering recorded coverage data for project-relevant entries"
                               VERBATIM)
            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_REPORT_DIR}
                               COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_REPORT_DIR}
                               COMMAND ${GENHTML_EXECUTABLE} --legend --show-details -t "${PROJECT_NAME} test coverage" -o ${COVERAGE_REPORT_DIR} ${COVERAGE_FILTERED_FILE}
                               DEPENDS ${COVERAGE_FILTERED_FILE}
                               COMMENT "Generating HTML coverage report in ${COVERAGE_REPORT_DIR}"
                               VERBATIM)

            ADD_CUSTOM_TARGET( "${ARG_COVERAGE}_coverage_html"
                              DEPENDS ${COVERAGE_REPORT_DIR})

        ENDIF()

        # xml coverage report
        IF(GCOVR_FOUND)

            SET(COVERAGE_XML_FILE "${CMAKE_BINARY_DIR}/${ARG_COVERAGE}_coverage.xml")
            SET(COVERAGE_XML_COMMAND_FILE "${CMAKE_BINARY_DIR}/${ARG_COVERAGE}_coverage-xml.cmake")

            GET_FILENAME_COMPONENT (COVERAGE_OBJECT_DIRECTORY ${ARG_BINARY_DIR} ABSOLUTE)

            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_XML_FILE}
                               COMMAND ${GCOVR_EXECUTABLE} -e unittest -x -r ${CMAKE_SOURCE_DIR} --object-directory=${COVERAGE_OBJECT_DIRECTORY} -o ${COVERAGE_XML_FILE}
                               COMMENT "Generating coverage XML report for ${ARG_COVERAGE}"
                               DEPENDS ${ARG_TARGETS}
                               WORKING_DIRECTORY ${ARG_BINARY_DIR}
                               VERBATIM)

            ADD_CUSTOM_TARGET( "${ARG_COVERAGE}_coverage_xml"
                              DEPENDS ${COVERAGE_XML_FILE})

        ENDIF()


        # provide a global coverage target executing both steps if available
        SET(GLOBAL_DEPENDS "")
        IF(LCOV_FOUND)
            LIST(APPEND GLOBAL_DEPENDS "${ARG_COVERAGE}_coverage_html")
        ENDIF()
        IF(GCOVR_FOUND)
            LIST(APPEND GLOBAL_DEPENDS "${ARG_COVERAGE}_coverage_xml")
        ENDIF()
        IF(TOOL_FOUND)
            ADD_CUSTOM_TARGET( "${ARG_COVERAGE}_coverage" DEPENDS ${GLOBAL_DEPENDS})
        ENDIF()

    ENDIF()

ENDFUNCTION()
