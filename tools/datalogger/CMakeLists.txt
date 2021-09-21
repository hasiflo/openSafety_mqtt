CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (libdatalogger)

SET ( CRC_SRCS
    ${PROJECT_SOURCE_DIR}/datalogger.h
    ${PROJECT_SOURCE_DIR}/datalogger.c
)
ADD_LIBRARY ( datalogger ${LIB_TYPE} ${CRC_SRCS} )

INSTALL ( TARGETS datalogger
            RUNTIME DESTINATION lib
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib/static
         )

INSTALL (DIRECTORY ${PROJECT_SOURCE_DIR}/ DESTINATION include/opensafety
            FILES_MATCHING PATTERN "*.h" )
