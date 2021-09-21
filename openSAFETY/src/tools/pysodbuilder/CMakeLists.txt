CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (pysodbuilder)

INCLUDE(FindPythonInterp)

#set variables for packages, paths and sourcefiles
SET ( PYSODBUILDER_PYXB "pyxb" )
SET ( PYSODBUILDER_COGAPP "cogapp" )

SET ( PYSODBUILDER_SETUP_PY "${PROJECT_BINARY_DIR}/setup.py" )
SET ( PYSODBUILDER_SOURCEDIST_TIMESTAMP
    "${CMAKE_CURRENT_BINARY_DIR}/sourcedisttimestamp" )
SET ( PYSDOBUILDER_COPY_TIMESTAMP "${CMAKE_CURRENT_BINARY_DIR}/copytimestamp" )
SET ( PYSODBUILDER_SOURCE_DIR "${PROJECT_SOURCE_DIR}/pysodb" )

SET ( PYSODBUILDER_SCRIPT "${PROJECT_BINARY_DIR}/scripts/pysodbuilder.py" )
SET ( PYSODBUILDER_MAIN "${PROJECT_BINARY_DIR}/pysodb/pysodbmain.py" )
SET ( PYSODBUILDER_BINDING_FILE
    "${PROJECT_BINARY_DIR}/pysodb/binding/xosddbinding.py" )

CONFIGURE_FILE ( "${PROJECT_SOURCE_DIR}/pysodb/core/versioninfo.py.in"
    "${PROJECT_BINARY_DIR}/pysodb/core/versioninfo.py" )

SET ( PYSODBUILDER_SOURCE
    ${PYSODBUILDER_SOURCE_DIR}/pysodbmain.py

    ${PYSODBUILDER_SOURCE_DIR}/binding/README.txt
    ${PYSODBUILDER_SOURCE_DIR}/binding/__init__.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/codegenutil.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/common.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/c_code_util.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/c_target_data.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/genappc.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/geneplscfgh.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/gensodc.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/gensodutilh.py
    ${PYSODBUILDER_SOURCE_DIR}/codegeneration/__init__.py
    ${PYSODBUILDER_SOURCE_DIR}/core/argparser.py
    ${PYSODBUILDER_SOURCE_DIR}/core/codegenerator.py
    ${PYSODBUILDER_SOURCE_DIR}/core/common.py
    ${PYSODBUILDER_SOURCE_DIR}/core/configparser.py
    ${PYSODBUILDER_SOURCE_DIR}/core/errorhandler.py
    ${PYSODBUILDER_SOURCE_DIR}/core/filehandling.py
    ${PYSODBUILDER_SOURCE_DIR}/core/util.py
    ${PYSODBUILDER_SOURCE_DIR}/core/versioninfo.py.in
    ${PYSODBUILDER_SOURCE_DIR}/core/__init__.py

    ${PYSODBUILDER_SOURCE_DIR}/osddprocessing/base.py
    ${PYSODBUILDER_SOURCE_DIR}/osddprocessing/mainprocessing.py
    ${PYSODBUILDER_SOURCE_DIR}/osddprocessing/postprocessing.py
    ${PYSODBUILDER_SOURCE_DIR}/osddprocessing/preprocessing.py
    ${PYSODBUILDER_SOURCE_DIR}/osddprocessing/processingdata.py
    ${PYSODBUILDER_SOURCE_DIR}/osddprocessing/__init__.py

    ${PYSODBUILDER_SOURCE_DIR}/util/constants.py
    ${PYSODBUILDER_SOURCE_DIR}/util/defaultsod.py
    ${PYSODBUILDER_SOURCE_DIR}/util/macro.py
    ${PYSODBUILDER_SOURCE_DIR}/util/osdd_data.py
    ${PYSODBUILDER_SOURCE_DIR}/util/sod_data.py
    ${PYSODBUILDER_SOURCE_DIR}/util/stringfunctions.py
    ${PYSODBUILDER_SOURCE_DIR}/util/structure.py
    ${PYSODBUILDER_SOURCE_DIR}/util/type_conversion.py
    ${PYSODBUILDER_SOURCE_DIR}/util/__init__.py
    ${PYSODBUILDER_SOURCE_DIR}/__init__.py

    ${PROJECT_SOURCE_DIR}/setup.py
    ${PROJECT_SOURCE_DIR}/scripts/pysodbuilder.py
)

SET ( PYSODBUILDER_NECESSARY_APPS
    ${PYSODBUILDER_PYXB}
    ${PYSODBUILDER_COGAPP}
)

# set (file) paths to demo files, if the variable DEMO_ENABLE is true
PySODBuilder_Set_Demo_Paths()

# try to locate needed third party python packages / modules in standard paths
PySODBuilderCheckPythonModules (
    "${PYSODBUILDER_NECESSARY_APPS}"
    )

# copy pysodbuilder to build directory
PySODBuilderCopy (
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
    "${PYSDOBUILDER_COPY_TIMESTAMP}"
    "${PYSODBUILDER_SOURCE}"
)

SET ( BINDING_DEPENDENCIES
    ${PYSDOBUILDER_COPY_TIMESTAMP}
    ${PYSODBUILDER_OSDD_SCHEMA_FILE})

# generate binding file based on given OSDD file
PySODBuilderGenerateBinding (
    "${PYSODBUILDER_BINDING_FILE}"
    "${PYSODBUILDER_OSDD_SCHEMA_FILE}"
    "${BINDING_DEPENDENCIES}"
    "${PROJECT_BINARY_DIR}"
)

#set variables and default values for gui and pysodbsettings.ini
#eplsCfg settings
SET ( PYSODBUILDER_MAXINSTANCES "1" CACHE STRING "max instances" )
SET ( PYSODBUILDER_MAXPAYLOADLENGTH "8" CACHE STRING "maxPayloadLength" )
SET ( PYSODBUILDER_ERRORSTRING "False" CACHE BOOL "errorstring" )
SET ( PYSODBUILDER_ERRORSTATISTIC "False" CACHE BOOL "errorstatistic" )
SET ( PYSODBUILDER_SCM "False" CACHE INTERNAL "scm" )
SET ( PYSODBUILDER_APPLICATIONOBJECTS "False" CACHE BOOL "applicationObjects" )
SET ( PYSODBUILDER_SAPL_REPORT_STATE_CHANGE "False" CACHE BOOL
    "saplReportStateChange" )

#constants settigs
SET ( PYSODBUILDER_CONFIGSTRING "demo" CACHE STRING
    "configuration string" )

#settings spdocfg
SET ( PYSODBUILDER_FRAMECOPYINTERN "False" CACHE BOOL "frameCopy" )
SET ( PYSODBUILDER_NUMLOOKUPTABLE "False" CACHE BOOL "numLookupTable" )
SET ( PYSODBUILDER_CONNECTIONVALIDBITFIELD "True" CACHE BOOL
    "connectionValidBitField" )
SET ( PYSODBUILDER_CONNECTIONVALIDSTATISTIC "False" CACHE BOOL
    "connectionValidStatistic" )
SET ( PYSODBUILDER_MAXRXSYNCEDPERTX "1" CACHE STRING "maxRxSyncedPerTx" )
SET ( PYSODBUILDER_NOTANSWEREDTR "100" CACHE STRING "notAnsweredTr" )
SET ( PYSODBUILDER_PROP_DELAY_STATISTIC "False" CACHE BOOL
    "propDelayStatistic" )
SET ( PYSODBUILDER_40BIT_CT_SUPPORT "True" CACHE BOOL "spdo40bitCTSupport" )
SET ( PYSODBUILDER_EXTENDED_CT_BITFIELD "False" CACHE BOOL
    "spdoExtendedCTBitField" )

#txspdocom settings
SET ( PYSODBUILDER_TX_MAXMAPENTRIES "4" CACHE STRING "maxMapEntries" )
SET ( PYSODBUILDER_TX_MAXPAYLOADSIZE "4" CACHE STRING "maxPayloadSize" )
SET ( PYSODBUILDER_TX_SPDOSACTIVATEDPERCALL "1" CACHE STRING
    "spdosActivatedPerCall" )
SET ( PYSODBUILDER_TX_MAXSPDO "1" CACHE STRING "maxSpdo" )
SET ( PYSODBUILDER_TX_MAXSPDOSDG "0" CACHE STRING "maxSpdoSdg" )

#rxspdocom settings
SET ( PYSODBUILDER_RX_MAXMAPENTRIES "4" CACHE STRING "maxMapEntries" )
SET ( PYSODBUILDER_RX_MAXPAYLOADSIZE "4" CACHE STRING "maxPayloadSize" )
SET ( PYSODBUILDER_RX_SPDOSACTIVATEDPERCALL "1" CACHE STRING
    "spdosActivatedPerCall" )
SET ( PYSODBUILDER_RX_MAXSPDO "1" CACHE STRING "maxSpdo" )
SET ( PYSODBUILDER_RX_MAXSPDOSDG "0" CACHE STRING "maxSpdoSdg" )

#scmcfg settings
SET ( PYSODBUILDER_MAXNODES "0" CACHE STRING "maxNodes" )
SET ( PYSODBUILDER_PROCESSEDNODESPERCALL "0" CACHE STRING
    "processedNodesPerCall" )
SET ( PYSODBUILDER_MAXSADR "0" CACHE STRING "maxSadr" )

CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/pysodbsettings.ini.in"
                "${PROJECT_BINARY_DIR}/pysodbsettings.ini" )

# add pysodbuilder target
PySODBuilder ( "${PYSODBUILDER_INPUT_DIR}" "${PYSODBUILDER_OUTPUT_DIR}"
            "${PYSODBUILDER_OSDD_FILE}" "${PYSODBUILDER_SETTINGS_FILE}"
            "${PYSODBUILDER_MODULE_ID}" "${PYSODBUILDER_BINDING_FILE}"
            "${PROJECT_BINARY_DIR}"
)


INSTALL( CODE
    "EXECUTE_PROCESS(
        COMMAND ${PYTHON_EXECUTABLE} ${PYSODBUILDER_SETUP_PY} install
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )"
)
