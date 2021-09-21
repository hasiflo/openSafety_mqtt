 
cmake_minimum_required(VERSION 2.8)

# set the project name
project(OpenSafety_SCM)


SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler
SET(CMAKE_C_COMPILER $ENV{HOME}/toolchains/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER $ENV{HOME}/toolchains/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++)

# Where is the target environment
SET(CMAKE_FIND_ROOT_PATH $ENV{HOME}/toolchains/rpi/rootfs)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")

# Search for programs only in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers only in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

INCLUDE_DIRECTORIES($ENV{HOME}/toolchains/rpi/rootfs/opt/vc/include)
INCLUDE_DIRECTORIES($ENV{HOME}/toolchains/rpi/rootfs/opt/vc/include/interface/vcos/pthreads)
INCLUDE_DIRECTORIES($ENV{HOME}/toolchains/rpi/rootfs/opt/vc/include/interface/vmcs_host/linux)
INCLUDE_DIRECTORIES(/home/florian/toolchains/rpi/rootfs/usr/lib/arm-linux-gnueabihf)



SET( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake"
                       ${CMAKE_MODULE_PATH})
INCLUDE(FindSourceFiles)
include(CheckIncludeFile)
include(CheckIncludeFiles)

INCLUDE_DIRECTORIES(/home/florian/toolchains/rpi/rootfs/usr/local/include)



########################################################################
# Set CFLAGS depending on build type
########################################################################
IF(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR ${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    SET(DBG_MODE _DEBUG)
    SET(CFG_APP_DEBUG_LVL 0xEC000000L CACHE STRING "Set the level of the debug output")
ELSE()
    #All other builds are release builds
    SET(DBG_MODE NDEBUG)
    UNSET(CFG_APP_DEBUG_LVL CACHE)
    SET(CFG_APP_DEBUG_LVL 0x00000000L)
ENDIF()
ADD_DEFINITIONS(-D_DEBUG)

#########################################
#Include System Libraries
#########################################
LINK_DIRECTORIES(/home/florian/toolchains/rpi/rootfs/usr/local/lib/)
########################################################################
# Set paths
SET(OSFT_BASE_DIR ${CMAKE_SOURCE_DIR}/openSAFETY)
SET(TOOLS_BASE_DIR ${CMAKE_SOURCE_DIR}/tools)
SET(SN_BASE_DIR ${OSFT_BASE_DIR}/src/eplssrc/SN)
SET(SCM_BASE_DIR ${OSFT_BASE_DIR}/src/eplssrc/SCM)
SET(CONTRIB_BASE_DIR ${OSFT_BASE_DIR}/src/eplssrc/contrib)
SET(OSCHECKSUM_BASE_DIR ${OSFT_BASE_DIR}/src/tools/oschecksum)

IF(CMAKE_TARGET STREQUAL "scm")
	SET(SAPL_BASE_DIR ${CMAKE_SOURCE_DIR}/SCM/SAPL)
ELSEIF(CMAKE_TARGET STREQUAL "sn1")
	SET(SAPL_BASE_DIR ${CMAKE_SOURCE_DIR}/SN1/SAPL)
ELSEIF(CMAKE_TARGET STREQUAL "sn2")
	SET(SAPL_BASE_DIR ${CMAKE_SOURCE_DIR}/SN2/SAPL)
ENDIF()



########################################################################
# Set all source files for SN
########################################################################
FindSourceFiles("${SN_BASE_DIR}" OSFT_SN_SRCS)
SET (SN_SRCS_SOD_C ${DEMO_CONFIG_DIR}/sn/sod.c)
SET (DEMO_CONFIG_DIR_SN ${DEMO_CONFIG_DIR}/sn)

########################################################################
# Set all source files for SCM
########################################################################
FindSourceFiles("${SCM_BASE_DIR}" OSFT_SCM_SRCS)

SET(DATA_SRCS
    ${TOOLS_BASE_DIR}/datalogger/datalogger.c
)

#SAPL Files
FindSourceFiles("${SAPL_BASE_DIR}" SAPL_SRCS)

#SHNF Files
SET(SHNF_TIME_SRCS
	${CMAKE_SOURCE_DIR}/SHNF_Time/Time.c
)

#SHNF UDP Files
FindSourceFiles("${CMAKE_SOURCE_DIR}/SHNF_UDP" SHNF_UDP_SRCS)

#SHNF MQTT Files
FindSourceFiles("${CMAKE_SOURCE_DIR}/SHNF_MQTT" SHNF_MQTT_SRCS)

SET(TOOLS_SRCS
    ${TOOLS_BASE_DIR}/SAPLscmmain.c
)

#Tools CRC Files
#FindSourceFiles("${TOOLS_BASE_DIR}/oschecksum" CRC_SRCS)
SET(CRC_SRCS
	${TOOLS_BASE_DIR}/oschecksum/crc_protocol.c
	${TOOLS_BASE_DIR}/oschecksum/crc.c
	
)


SET(BASE_SRCS

                ${MAINS_SRC}
                ${OSFT_SN_SRCS}
                ${LIBOSCHECKSUM_SRCS}
                ${DATA_SRCS}
                ${SAPL_SRCS}
                ${SHNF_TIME_SRCS}
                #${SHNF_UDP_SRCS}
                ${SHNF_MQTT_SRCS}
                ${CRC_SRCS}
)

SET(SCM_SRCS
    ${OSFT_SCM_SRCS}
	${BASE_SRCS}
)

SET(SN_SRCS
	${BASE_SRCS}
)

########################################################################
# Set include paths
########################################################################
SET(BASE_INCS
    ${CMAKE_SOURCE_DIR}/SHNF_Time
    #${CMAKE_SOURCE_DIR}/SHNF_UDP
    ${CMAKE_SOURCE_DIR}/SHNF_MQTT
    ${CMAKE_SOURCE_DIR}/SHNF_NetworkCommon
    ${CMAKE_SOURCE_DIR}/SAPL
    ${SN_BASE_DIR}
    ${CONTRIB_BASE_DIR}
    ${OSCHECKSUM_BASE_DIR}/include
    ${TOOLS_BASE_DIR}/datalogger
    ${TOOLS_BASE_DIR}/oschecksum/include/oschecksum
)

SET(SCM_INCS
    ${SCM_BASE_DIR}
	${CMAKE_SOURCE_DIR}/SCM/include
	${BASE_INCS}
	
)

SET(SN1_INCS
	${CMAKE_SOURCE_DIR}/SN1/include
	${BASE_INCS}
    
)

SET(SN2_INCS
	${CMAKE_SOURCE_DIR}/SN2/include
	${BASE_INCS}
    
)




########################################################################
# Set include paths
########################################################################
IF(CMAKE_TARGET STREQUAL "scm")
	INCLUDE_DIRECTORIES(
	                    ${SCM_INCS}
	)


	# add the executable
	add_executable(${CMAKE_TARGET} ${SAPL_BASE_DIR}/SAPLscmmain.c ${SCM_SRCS})
	#link pthread lib
ELSEIF(CMAKE_TARGET STREQUAL "sn1")
	INCLUDE_DIRECTORIES(
	                    ${SN1_INCS}
	)
	# add the executable
	add_executable(${CMAKE_TARGET} ${SAPL_BASE_DIR}/SAPLsnmain.c ${SN_SRCS})
	#link pthread lib
ELSEIF(CMAKE_TARGET STREQUAL "sn2")
	INCLUDE_DIRECTORIES(
	                    ${SN2_INCS}
	)
	# add the executable
	add_executable(${CMAKE_TARGET} ${SAPL_BASE_DIR}/SAPLsnmain.c ${SN_SRCS})
	#link pthread lib
ENDIF()

target_link_libraries(${CMAKE_TARGET} pthread)
target_link_libraries(${CMAKE_TARGET} paho-mqtt3a)

