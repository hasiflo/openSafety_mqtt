CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (liboschecksum)

SET ( CRC_SRCS
    ${PROJECT_SOURCE_DIR}/include/oschecksum/crc.h
    ${PROJECT_SOURCE_DIR}/crc.c
    ${PROJECT_SOURCE_DIR}/crc_protocol.c
)

SET ( TABLE_CREATE_SRCS
   ${PROJECT_SOURCE_DIR}/tablecreate.c
)

INCLUDE_DIRECTORIES ( ${PROJECT_SOURCE_DIR}/include/
                      ${PROJECT_BINARY_DIR}/include
)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/include/oschecksum/config.h.in ${PROJECT_BINARY_DIR}/include/oschecksum/config.h)

ADD_LIBRARY ( oschecksum ${LIB_TYPE} ${CRC_SRCS} )

ADD_EXECUTABLE( crccreate EXCLUDE_FROM_ALL ${TABLE_CREATE_SRCS} )
ADD_DEPENDENCIES ( crccreate "oschecksum" )
TARGET_LINK_LIBRARIES ( crccreate "oschecksum" )

INSTALL ( TARGETS oschecksum
            RUNTIME DESTINATION lib
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib/static
         )

INSTALL (DIRECTORY ${PROJECT_SOURCE_DIR}/ DESTINATION include/opensafety/oschecksum
            FILES_MATCHING PATTERN "include/oschecksum/*.h" )
