CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (tstoschecksum)

SET ( TST_SOURCES
    ${PROJECT_SOURCE_DIR}/../../common/cunit_main.c
    ${PROJECT_SOURCE_DIR}/Driver/TST_oschecksum.c

    ${PROJECT_SOURCE_DIR}/Driver/TST_oschecksum_Unit_CRC8.c
    ${PROJECT_SOURCE_DIR}/Driver/TST_oschecksum_Unit_CRC16.c
    ${PROJECT_SOURCE_DIR}/Driver/TST_oschecksum_Unit_CRC32.c
    ${PROJECT_SOURCE_DIR}/Driver/TST_oschecksum_paramcrc.c
)

INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/contrib/cunit/include" )
INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/contrib/osswin" )
INCLUDE_DIRECTORIES ( "${PROJECT_SOURCE_DIR}/../../common" )
INCLUDE_DIRECTORIES ( "${PROJECT_SOURCE_DIR}" )
INCLUDE_DIRECTORIES ( "${PROJECT_SOURCE_DIR}/Driver" )
INCLUDE_DIRECTORIES ( "${liboschecksum_SOURCE_DIR}/include" )

SET ( ADD_ADDITIONAL_LIBRARIES
  oschecksum
)
IF(WIN32)
    SET(ADD_ADDITIONAL_LIBRARIES "${ADD_ADDITIONAL_LIBRARIES}"
                                 "osswin"
    )

    INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/contrib/osswin" )
ENDIF()

SimpleTest ( "TSToschecksum" "tstoschecksum" "${TST_SOURCES}" )
TARGET_LINK_LIBRARIES ( "tstoschecksum" ${ADD_ADDITIONAL_LIBRARIES} )
ADD_DEPENDENCIES ( "tstoschecksum" ${ADD_ADDITIONAL_LIBRARIES} )

IF ( ( WIN32 ) AND ( ${LIB_TYPE} STREQUAL "SHARED" ) )
    EnsureLibraries(tstoschecksum "${ADD_ADDITIONAL_LIBRARIES}")
ENDIF ( ( WIN32 ) AND ( ${LIB_TYPE} STREQUAL "SHARED" ) )

# Enable Coverage on target and test subject
AddCoverage ( "EPLS" "oschecksum" )
AddCoverage ( "EPLS" "tstoschecksum" )
