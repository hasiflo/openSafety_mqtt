CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

PROJECT (openSAFETY)

SET( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake" ${CMAKE_MODULE_PATH})

INCLUDE(CMakeOptions.txt)

INCLUDE(AddTarget)
INCLUDE(InstallRequiredSystemLibraries)

IF ( TARGET_GENERIC AND NOT BUILD_OPENSAFETY_TOOLS )
    INCLUDE(CPack)
ENDIF ( TARGET_GENERIC AND NOT BUILD_OPENSAFETY_TOOLS )

INCLUDE(SetTargetInclude)
INCLUDE(CheckTypeSize)
INCLUDE(EnsureLibraries)
INCLUDE(CopyFileToTarget)
INCLUDE(FindSourceFiles)
INCLUDE(FindDataTypesForPlattform)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckFunctionExists)
INCLUDE(AddSPLint OPTIONAL)
INCLUDE(FindPythonInterp)
INCLUDE(FindDoxygen OPTIONAL)

IF ( BUILD_OPENSAFETY_PYTHON_TOOLS )
    INCLUDE (LocatePythonModule)
ENDIF()

IF ( NOT PYTHONINTERP_FOUND AND CHECKSUM_SRC_ENABLE_CHECKS )
    MESSAGE ( FATAL_ERROR "CRC Sourcecode checks require a python interpreter, which was not found!" )
ENDIF()

IF ( CHECKSUM_SRC_ENABLE_CHECKS AND NOT BUILD_OPENSAFETY_TOOLS )
    MESSAGE ( FATAL_ERROR "CRC Sourcecode checks require the opensafety tools" )
ENDIF()

IF ( UNITTEST_ENABLE )

    CONFIGURE_FILE (
        ${CMAKE_SOURCE_DIR}/CTestCustom.cmake
        ${CMAKE_BINARY_DIR}/CTestCustom.cmake
    )

    ADD_DEFINITIONS ( " -D__SACUN__" )
    ADD_DEFINITIONS ( " -D__SACUN_SCM_VIRTUALSN" )

    IF ( UNITTEST_XML_REPORTS )
        ADD_DEFINITIONS ( " -DCUNIT_AUTOMATED " )
    ENDIF ( UNITTEST_XML_REPORTS )
ENDIF ( UNITTEST_ENABLE )

IF ( CMAKE_BUILD_TYPE STREQUAL "coverage" AND UNITTEST_ENABLE )
    IF ( CMAKE_COMPILER_IS_GNUCXX )

        FIND_PACKAGE(lcov)
        FIND_PACKAGE(gcov)
        FIND_PACKAGE(gcovr)

        SET ( COVERAGE_EPLS CACHE INTERNAL "" FORCE )

        IF ( PYTHONINTERP_FOUND )
            INCLUDE(EnableCoverageReport OPTIONAL)
        ENDIF ()

        IF ( NOT GCOVR_FOUND AND NOT GCOV_FOUND )
            MESSAGE ( STATUS "Disabling coverage tests, gcovr not found in path" )
        ENDIF ( NOT GCOVR_FOUND AND NOT GCOV_FOUND )

    ELSE ( CMAKE_COMPILER_IS_GNUCXX )
        MESSAGE ( FATAL_ERROR "Coverage is currently only possible using gcov and gnu compiler suite" )
    ENDIF ( CMAKE_COMPILER_IS_GNUCXX )
ELSE()
    MESSAGE ( STATUS "Build type not suitable for coverage" )
ENDIF ( CMAKE_BUILD_TYPE STREQUAL "coverage" AND UNITTEST_ENABLE )

SET ( CMAKE_REQUIRED_INCLUDES "/usr/include" )

CHECK_INCLUDE_FILE ( "sys/socket.h" HAVE_SYS_SOCKET_H )
IF ( WIN32 )
    CHECK_INCLUDE_FILE ( "winsock2.h" HAVE_WINSOCK2_H )
    CHECK_INCLUDE_FILE ( "bits/predefs.h" HAVE_BITS_PREDEFS_H )

IF ( MSVC )
    # Disabling warnings for padding and deprecated functions
    SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4820 /D_CRT_SECURE_NO_DEPRECATE" )
    # Disabling warnings for padding and deprecated functions
    SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4820 /D_CRT_SECURE_NO_DEPRECATE" )
ENDIF ( MSVC )
ENDIF ( WIN32 )
CHECK_INCLUDE_FILE ( "netinet/in.h" HAVE_NETINET_IN_H )
CHECK_INCLUDE_FILE ( "arpa/inet.h" HAVE_ARPA_INET_H )
CHECK_INCLUDE_FILE ( "features.h" HAVE_FEATURES_H )
CHECK_INCLUDE_FILE ( "arpa/nameser.h" HAVE_ARPA_NAMESER_H )
CHECK_INCLUDE_FILE ( "sys/param.h" HAVE_SYS_PARAM_H )
CHECK_INCLUDE_FILE ( "sys/types.h" HAVE_SYS_TYPES_H )
CHECK_INCLUDE_FILE ( "sys/libgen.h" HAVE_LIBGEN_H )
CHECK_INCLUDE_FILE ( "stdarg.h" HAVE_STDARG_H )
CHECK_INCLUDE_FILE ( "stdint.h" HAVE_STDINT_H )
CHECK_INCLUDE_FILE ( "stdbool.h" HAVE_STDBOOL_H )

CHECK_FUNCTION_EXISTS( "inet_pton" HAVE_INET_PTON )

CHECK_FUNCTION_EXISTS( "snprintf" HAVE_SNPRINTF )
CHECK_FUNCTION_EXISTS( "strdup" HAVE_STRDUP )
CHECK_FUNCTION_EXISTS( "vsnprintf" HAVE_VSNPRINTF )
CHECK_FUNCTION_EXISTS( "_snprintf" HAVE__SNPRINTF )

SET ( WIN32_EXECUTABLE "" )

# Set the LIB_TYPE variable to STATIC
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  # User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)

INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/tools")
INCLUDE_DIRECTORIES ("${PROJECT_BINARY_DIR}")

IF ( TARGET_WINDOWS )
    SET ( WIN32_EXECUTABLE "WIN32" )
    IF (CMAKE_COMPILER_IS_GNUCC)
        MESSAGE ( STATUS "Cross-compiling for Windows" )
        ADD_DEFINITIONS ( -DWIN32 -D"_WIN32_WINNT=0x0501" )
    ELSE (CMAKE_COMPILER_IS_GNUCC)
        MESSAGE ( STATUS "Building on Windows" )
        ADD_DEFINITIONS ( /D"_WIN32_WINNT=0x0501" )
    ENDIF (CMAKE_COMPILER_IS_GNUCC)
    ADD_SUBDIRECTORY ( "${PROJECT_SOURCE_DIR}/contrib/" )

ELSE ( TARGET_WINDOWS )

    IF ( CYGWIN )
        MESSAGE ( STATUS "Building on Cygwin/Windows" )
    ELSE ( CYGWIN )
        MESSAGE ( STATUS "Building on Linux" )

        #IF ( NOT HAVE_FEATURES_H )
        #    SET ( X86_64_INCLUDE "/usr/include/x86_64-linux-gnu/" )
        #
        #    IF ( IS_DIRECTORY ${X86_64_INCLUDE} )
        #    MESSAGE ( FATAL_ERROR "This is a known issue, you lack libc6-dev-i386 on your system" )
        #    ENDIF ( IS_DIRECTORY ${X86_64_INCLUDE} )
        #ENDIF ( NOT HAVE_FEATURES_H )

    ENDIF ( CYGWIN )

ENDIF ( TARGET_WINDOWS )

# configure a header file to pass some of the CMake settings
# to the source code
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

INSTALL ( FILES "${PROJECT_BINARY_DIR}/config.h" DESTINATION include/opensafety )


#######################
## CRC
#######################

IF ( BUILD_OPENSAFETY_TOOLS )
    ADD_SUBDIRECTORY ( "${PROJECT_SOURCE_DIR}/tools/oschecksum" )
    ADD_SUBDIRECTORY ( "${PROJECT_SOURCE_DIR}/tools/oscrccheck" )
ENDIF ( BUILD_OPENSAFETY_TOOLS )

IF ( BUILD_OPENSAFETY_LIBS )
    SET(OSDD_XSD "${CMAKE_SOURCE_DIR}/../doc/XOSDD_R15.xsd")
    SET(OSDD_LANG_XSD "${CMAKE_SOURCE_DIR}/../doc/XOSDDLocalize_R15.xsd")

    # Copy the OSDD XDS files to the CMAKE_BINARY_DIR!
    # This is needed by programs who execute
    # the osdd related programs in this folder. (e.g: Valgrind)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OSDD_XSD}" "${CMAKE_BINARY_DIR}"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OSDD_LANG_XSD}" "${CMAKE_BINARY_DIR}"
                    RESULT_VARIABLE COPY_RES
                    ERROR_VARIABLE COPY_STDERR
    )

    IF( NOT ${COPY_RES} MATCHES "0" )
        MESSAGE ( FATAL_ERROR "${CMAKE_COMMAND} returned error: ${COPY_STDERR}" )
    ENDIF()

ENDIF ( BUILD_OPENSAFETY_LIBS )

IF ( BUILD_OPENSAFETY_TOOLS AND CHECKSUM_SRC_ENABLE_CHECKS )

    IF ( CHECKSUM_SRC_PERFORM_CHECKS )

        INCLUDE(CheckOSChecksum)
        FindSourceFiles("${CMAKE_SOURCE_DIR}/eplssrc/SCM" SCM_SOURCE)
        FindSourceFiles("${CMAKE_SOURCE_DIR}/eplssrc/SN" SN_SOURCE)

        SET(oschecksumLocation  $<TARGET_FILE:oscrccheck>)
        CheckCRCCode ( "SCM" "${openSAFETY_STACK_FILE}" "${SCM_SOURCE}" "${oschecksumLocation}" )
        CheckCRCCode ( "SN" "${openSAFETY_STACK_FILE}" "${SN_SOURCE}" "${oschecksumLocation}" )

        MESSAGE ( STATUS "openSAFETY Stack CRC Checksum: ${openSAFETY_STACK_CRC}" )

    ENDIF ( CHECKSUM_SRC_PERFORM_CHECKS )

ENDIF ( BUILD_OPENSAFETY_TOOLS AND CHECKSUM_SRC_ENABLE_CHECKS )

#######################
## TOOLS
#######################

IF ( BUILD_OPENSAFETY_TOOLS )
    IF ( BUILD_OPENSAFETY_PYTHON_TOOLS )
        INCLUDE (PySODBuilder)
        ADD_SUBDIRECTORY ( "${PROJECT_SOURCE_DIR}/tools/pysodbuilder" )
    ENDIF()
ENDIF ( BUILD_OPENSAFETY_TOOLS )

#######################
## DOXYGEN
#######################

IF ( DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND AND BUILD_OPENSAFETY_DOCUMENTATION )

    SET ( DOXYFILE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/doc" )
    SET ( DOXYFILE_IMAGE_DIR "${CMAKE_SOURCE_DIR}/../doc" )
    SET ( DOXYFILE_DOC_DIR "${CMAKE_SOURCE_DIR}/../doc" )
    SET ( DOXYFILE_SOURCE_DIR "${CMAKE_SOURCE_DIR}" )

    IF ( DOCUMENTATION_GENERATE_LATEX )
        SET ( DOXYFILE_LATEX "YES" )
    ELSE ( DOCUMENTATION_GENERATE_LATEX )
        SET ( DOXYFILE_LATEX "NO" )
    ENDIF ( DOCUMENTATION_GENERATE_LATEX )

    INCLUDE(UseDoxygen OPTIONAL)
ELSE()
    IF ( BUILD_OPENSAFETY_DOCUMENTATION )
        IF ( NOT DOXYGEN_FOUND )
            MESSAGE ( WARNING "Documentation will not be build, as no doxygen installation could be found!" )
        ELSEIF ( NOT DOXYGEN_DOT_FOUND )
            MESSAGE ( STATUS "No DOT graphics will be generated, graphviz package is not installed" )
        ENDIF()
    ENDIF()
ENDIF ()

#######################
## Documentation
#######################

IF ( UNITTEST_ENABLE )

    ENABLE_TESTING()

    ADD_SUBDIRECTORY ( "${PROJECT_SOURCE_DIR}/contrib/cunit" )
    ADD_SUBDIRECTORY ( "${PROJECT_SOURCE_DIR}/unittest" )

    IF ( GCOVR_FOUND AND PYTHONINTERP_FOUND )
        ENABLE_COVERAGE_REPORT ( TARGETS ${COVERAGE_EPLS} COVERAGE "EPLS" BINARY_DIR "${CMAKE_BINARY_DIR}/unittest/" FILTER "unittest/*" )
    ENDIF ( GCOVR_FOUND AND PYTHONINTERP_FOUND )

ENDIF ( UNITTEST_ENABLE )
