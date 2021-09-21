# Locate and define datatypes for fix sizes
# Defines the following variables:
#
#   DATATYPE_INT8 - A 8-bit datatype for integer values
#   DATATYPE_INT16 - A 16-bit datatype for integer values
#   DATATYPE_INT32 - A 32-bit datatype for integer values
#   DATATYPE_INT64 - A 64-bit datatype for integer values
#
#   DATATYPE_FLOAT32 - A 32-bit datatype for floating-point values
#   DATATYPE_FLOAT64 - A 64-bit datatype for floating-point values
#

SET(CMAKE_EXTRA_INCLUDE_FILES "stdbool.h")
CHECK_TYPE_SIZE("bool" BOOL)
SET(CMAKE_EXTRA_INCLUDE_FILES )

SET(CMAKE_EXTRA_INCLUDE_FILES stdlib.h)
SET ( DATATYPE_CHAR "char" )

CHECK_TYPE_SIZE("short" SHORT)
CHECK_TYPE_SIZE("char" CHAR)
CHECK_TYPE_SIZE("byte" BYTE)
IF ( HAVE_SHORT EQUAL 1 )
    SET ( DATATYPE_INT8 "short" )
ELSEIF ( HAVE_BYTE EQUAL 1 )
    SET ( DATATYPE_INT8 "byte" )
ELSEIF ( HAVE_CHAR EQUAL 1 )
    SET ( DATATYPE_INT8 "char" )
ENDIF ( HAVE_SHORT EQUAL 1 )

CHECK_TYPE_SIZE("int" INT)
IF ( HAVE_SHORT EQUAL 2 )
    SET ( DATATYPE_INT16 "short" )
ELSEIF ( HAVE_INT EQUAL 2 )
    SET ( DATATYPE_INT16 "int" )
ENDIF ( HAVE_SHORT EQUAL 2 )

CHECK_TYPE_SIZE("long" LONG)
CHECK_TYPE_SIZE("int" INT)
IF ( HAVE_INT EQUAL 4 )
    SET ( DATATYPE_INT32 "int" )
ELSEIF ( HAVE_LONG EQUAL 4 )
    SET ( DATATYPE_INT32 "long" )
ENDIF ( HAVE_INT EQUAL 4 )

# Workaround for 64-Bit
# The current stack implementation is not able to handle real 64-bit mode.
# Problems occur, because the code tries to assign pointers (which have 8 byte in 64-bit)
# to UINT32 which are by nature only 32bit. Solution for now is to allow only 32bit target
# being built on Linux, which is handled by adding -m32 to gcc
IF(UNIX)
    IF (CMAKE_COMPILER_IS_GNUCC)
        # We have a 64 bit compiler, let's add a switch for 32 bit
        IF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            SET( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-m32 -Wformat=0")
        ENDIF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(UNIX)

# No known switch on win32 target
IF ( CMAKE_HOST_WIN32 )
    IF ( CMAKE_CL_64 )
        # We have a 64 bit compiler, let's add a switch for 32 bit
    ENDIF ( CMAKE_CL_64 )
ENDIF ( CMAKE_HOST_WIN32 )

IF (WIN32)
    CHECK_TYPE_SIZE("__int64" DLONG)
    IF ( HAVE_LONG EQUAL 8 )
        SET ( DATATYPE_INT64 "long" )
    ELSEIF ( HAVE_DLONG EQUAL 8 )
        SET ( DATATYPE_INT64 "__int64" )
    ENDIF ( HAVE_LONG EQUAL 8 )
ELSEIF (UNIX)
    CHECK_TYPE_SIZE("long long" DLONG)
    IF ( HAVE_LONG EQUAL 8 )
        SET ( DATATYPE_INT64 "long" )
    ELSEIF ( HAVE_DLONG EQUAL 8 )
        SET ( DATATYPE_INT64 "long long" )
    ENDIF ( HAVE_LONG EQUAL 8 )
ENDIF (WIN32)

CHECK_TYPE_SIZE("float" FLOAT)
IF ( HAVE_FLOAT EQUAL 4 )
    SET ( DATATYPE_FLOAT32 "float" )
ENDIF ( HAVE_FLOAT EQUAL 4 )

CHECK_TYPE_SIZE("double" DOUBLE)
IF ( HAVE_DOUBLE EQUAL 8 )
    SET ( DATATYPE_FLOAT64 "double" )
ENDIF ( HAVE_DOUBLE EQUAL 8 )

SET(CMAKE_EXTRA_INCLUDE_FILES)


# Workaround for 64-Bit
# The current stack implementation is not able to handle real 64-bit mode.
# Problems occur, because the code tries to assign pointers (which have 8 byte in 64-bit)
# to UINT32 which are by nature only 32bit. Solution for now is to allow only 32bit target
# being built on Linux, which is handled by adding -m32 to gcc
#
# One thing that does not build on 64-bit yet are the unittests for SOD, due to the
# static initialization of the test structures

IF ( UNIX )
    # We have a 64 bit compiler, let's add a switch for 32 bit
    IF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        MESSAGE ( STATUS "Setting 32bit compatibility on 64bit Linux" )

        macro (add_executable _name)
            # invoke built-in add_executable
            _add_executable(${ARGV})
            if (TARGET ${_name})
                GET_TARGET_PROPERTY ( TGT_LNK_FLG ${_name} LINK_FLAGS )
                GET_TARGET_PROPERTY(TARGET_COMPILE_FLAGS ${_name} COMPILE_FLAGS )
                IF ( "${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND" )
                SET ( TARGET_COMPILE_FLAGS "" )
                ENDIF ( "${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND" )
                SET ( TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS} -m32 " )

                GET_TARGET_PROPERTY(TARGET_LINK_FLAGS ${_name} LINK_FLAGS )
                IF ( "${TARGET_LINK_FLAGS}" STREQUAL "TARGET_LINK_FLAGS-NOTFOUND" )
                SET ( TARGET_LINK_FLAGS "" )
                ENDIF ( "${TARGET_LINK_FLAGS}" STREQUAL "TARGET_LINK_FLAGS-NOTFOUND" )
                SET ( TARGET_LINK_FLAGS "${TARGET_LINK_FLAGS} -m32 " )

                SET_TARGET_PROPERTIES(${_name} PROPERTIES COMPILE_FLAGS ${TARGET_COMPILE_FLAGS}
                                                LINK_FLAGS ${TARGET_LINK_FLAGS} )
            endif()
        endmacro()

        macro (add_library _name)
            # invoke built-in add_executable
            _add_library(${ARGV})
            if (TARGET ${_name})
                GET_TARGET_PROPERTY ( TGT_LNK_FLG ${_name} LINK_FLAGS )
                GET_TARGET_PROPERTY(TARGET_COMPILE_FLAGS ${_name} COMPILE_FLAGS )
                IF ( "${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND" )
                SET ( TARGET_COMPILE_FLAGS "" )
                ENDIF ( "${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND" )
                SET ( TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS} -m32 " )

                GET_TARGET_PROPERTY(TARGET_LINK_FLAGS ${_name} LINK_FLAGS )
                IF ( "${TARGET_LINK_FLAGS}" STREQUAL "TARGET_LINK_FLAGS-NOTFOUND" )
                SET ( TARGET_LINK_FLAGS "" )
                ENDIF ( "${TARGET_LINK_FLAGS}" STREQUAL "TARGET_LINK_FLAGS-NOTFOUND" )
                SET ( TARGET_LINK_FLAGS "${TARGET_LINK_FLAGS} -m32 " )

                SET_TARGET_PROPERTIES(${_name} PROPERTIES COMPILE_FLAGS ${TARGET_COMPILE_FLAGS}
                                                LINK_FLAGS ${TARGET_LINK_FLAGS} )
            endif()
        endmacro()

    ENDIF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )

ENDIF()