CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (CUnit)

SET ( CUNIT_SRCS
    ${PROJECT_SOURCE_DIR}/Common/Automated.c
    ${PROJECT_SOURCE_DIR}/Common/Basic.c
    ${PROJECT_SOURCE_DIR}/Common/CUError.c
    ${PROJECT_SOURCE_DIR}/Common/MyMem.c
    ${PROJECT_SOURCE_DIR}/Common/TestDB.c
    ${PROJECT_SOURCE_DIR}/Common/TestRun.c
    ${PROJECT_SOURCE_DIR}/Common/Util.c
    ${PROJECT_SOURCE_DIR}/Common/Console.c
    ${PROJECT_SOURCE_DIR}/include/cunit/CUnit.h
)

IF ( MSVC )
    SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996" )
ENDIF ( MSVC )

INCLUDE_DIRECTORIES ( ${PROJECT_SOURCE_DIR}/include/cunit )
ADD_LIBRARY ( CUnit ${LIB_TYPE} ${CUNIT_SRCS} )
