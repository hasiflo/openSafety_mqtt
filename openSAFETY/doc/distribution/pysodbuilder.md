pysodbuilder {#page_pysodbuilder}
------------

[TOC]

# User Guide {#sect_pysodbuilder_user_guide}

The user guide provides details about how to use this tool as part of the
**CMake** configuration.

## Requirements {#sect_pysodbuilder_requirements}

This tool is written in <a href="https://www.python.org/">Python</a>
and makes use of two third party packages.
For proper execution of the tool the required Python interpreter and packages
have to be installed:

 - Python interpreter version 2.7.9 or 3.4.2 https://www.python.org/ , the tool
  was developed and tested with both versions
 - PyXB 1.2.4 https://pypi.python.org/pypi/PyXB/1.2.4
 - cogapp 2.4 https://pypi.python.org/pypi/cogapp/2.4


## CMake Configuration Options {#sect_pysodbuilder_cmakeconfig}

The following build configuration options are available to pass to **CMake** by
using the `-D` parameter or by enabling them in the graphical user interface:

> **Note:** To enable the usage of the pysodbuilder tool, the option
> **BUILD_OPENSAFETY_PYTHON_TOOLS** has to be set.



### General Configuration Options {#sect_pysodbuilder_cmakeconfig_general}

The default values of the CMake options shown in the following table
are depending on the setting of **BUILD_OPENSAFETY_PYTHON_TOOLS**. If the setting is enabled,
the options will be preset to the default value and will be blank, otherwise.

Variable name           | Description                           | Default value
----------------------- | --------------------------------------|--------------
PYSODBUILDER_INPUT_DIR  | Directory of input template files     | [ProjectDir]/src/tools/pysodbuilder/demo_files/input
PYSODBUILDER_MODULE_ID  | Module id to generate files for, if the OSDD file contains several modules | -
PYSODBUILDER_OUTPUT_DIR | Directory of the output files         | [ProjectBuildDir]/tools/pysodbuilder/demo_files/output
PYSODBUILDER_OSDD_FILE  | OSDD file to use for pysodbuilder     | [ProjectDir]/src/tools/pysodbuilder/demo_files/osdd/demo.xosdd
PYSODBUILDER_OSDD_SCHEMA_FILE | OSDD schema file to which the used OSDD file adheres | [ProjectBuildDir]/XOSDD_R15.xsd, if existing, [ProjectDir]/doc/XOSDD_R15.xsd otherwise
PYSODBUILDER_SETTINGS_FILE | Settings file to use for pysodbuilder, which is created by **CMake** | [PysodbuilderBinaryDir]/pysodbsettings.ini


- **PYSODBUILDER_INPUT_DIR**

  Directory of input template files used for code generation

- **PYSODBUILDER_MODULE_ID**

  Module ID to generate files for, if the OSDD file contains several modules

- **PYSODBUILDER_OUTPUT_DIR**

  Directory of the generated output files

- **PYSODBUILDER_OSDD_FILE**

  OSDD file to use for pysodbuilder

- **PYSODBUILDER_OSDD_SCHEMA_FILE**

  OSDD schema file to which the used OSDD file adheres

- **PYSODBUILDER_SETTINGS_FILE**

  Settings file to use for pysodbuilder, which is created by **CMake**


### eplsCfg settings {#sect_pysodbuilder_cmakeconfig_eplscfg}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_SAPL_REPORT_STATE_CHANGE | Enable or disable status change callback functions | OFF
PYSODBUILDER_MAXINSTANCES     | Maximum number of instances               | 1
PYSODBUILDER_MAXPAYLOADLENGTH | Maximum payload data length of a SSDO     | 8
PYSODBUILDER_ERRORSTRING      | Enables or disables the  XXX_GetErrorStr function (e.g. SOD_GetErrorStr) | OFF
PYSODBUILDER_ERRORSTATISTIC   | Enable or disable the extended telegram error statistic | OFF
PYSODBUILDER_APPLICATIONOBJECTS | Enable or disable the objects managed by the application | OFF


- **PYSODBUILDER_SAPL_REPORT_STATE_CHANGE**
  Define to enable or disable status change callback functions
  These functions are may be used by the application to detect status changes.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa EPLS_cfg_SAPL_REPORT_STATE_CHANGE


- **PYSODBUILDER_MAXINSTANCES**

  Configures the maximum number of instances.
  Allowed values : 1..255
  \sa EPLS_cfg_MAX_INSTANCES

- **PYSODBUILDER_MAXPAYLOADLENGTH**

  Configures the maximum payload data length
  of a received SSDO Service Request or a transmitted SSDO Service Response on
  a SSDO server.

  **NOTE:**
  On a SSDO client the maximum payload data length of a SSDO server is
  specified in the SOD (Index 0xC400 sub-index 8 MaximumSsdoPayloadLen) and
  must be passed to the SSDO client by calling the SSDOC_SendWriteReq() and
  SSDOC_SendReadReq().

  Allowed values: 8..254

  **ATTENTION:** standard data type INT64, UINT64, REAL64
  is only available for EPLS_cfg_MAX_PYLD_LEN >= 12
  \sa EPLS_cfg_MAX_PYLD_LEN

- **PYSODBUILDER_ERRORSTRING**

  Enable or disable the XXX_GetErrorStr function
  (e.g. SOD_GetErrorStr). These functions are used to print an error string
  for the error code.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa EPLS_cfg_ERROR_STRING

- **PYSODBUILDER_ERRORSTATISTIC**

  Enable or disable the extended telegram error statistic.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa EPLS_cfg_ERROR_STATISTIC

- **PYSODBUILDER_APPLICATIONOBJECTS**

  Enable or disable the objects managed by the application

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SOD_cfg_APPLICATION_OBJ

### constants settings {#sect_pysodbuilder_cmakeconfig_constants}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_CONFIGSTRING     | Represents the EPLsafety Stack configuration as a string | demo

- **PYSODBUILDER_CONFIGSTRING**

  Represents the EPLsafety Stack configuration as a string
  \sa EPLS_k_CONFIGURATION_STRING

### spdocfg settings {#sect_pysodbuilder_cmakeconfig_spdocfg}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_PROP_DELAY_STATISTIC | Enable variable which delivers an average over the last cycle and actual cycle propagation delay | OFF
PYSODBUILDER_40BIT_CT_SUPPORT | Enable the support for SPDOs with a 40-bit CT value | ON
PYSODBUILDER_EXTENDED_CT_BITFIELD | Extended CT bit field is to be created | OFF
PYSODBUILDER_FRAMECOPYINTERN     | Enable or disable the copy of the received SPDO frames within the SPDO_ProcessRxSpdo() | OFF
PYSODBUILDER_NUMLOOKUPTABLE |  Enable or disable the usage of the Lookup table for the SPDO number assignment | OFF
PYSODBUILDER_CONNECTIONVALIDBITFIELD | Enable or disable creation of Valid bit field | ON
PYSODBUILDER_CONNECTIONVALIDSTATISTIC | Enable or disable creation of Valid statistic counter field created | OFF
PYSODBUILDER_MAXRXSYNCEDPERTX | Maximum number of RxSPDOs to be synchronized over a TxSPDO | 1
PYSODBUILDER_MAXRXSYNCEDPERTX | Number of not answered TR (internal Time Request counter) | 100

- **PYSODBUILDER_PROP_DELAY_STATISTIC**
  Define to enable variable which delivers an average over the last cycle
  and actual cycle propagation delay.

  \sa SPDO_cfg_PROP_DELAY_STATISTIC

- **PYSODBUILDER_40BIT_CT_SUPPORT**
  Define to enable the support for SPDOs with a 40-bit CT value.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SPDO_cfg_40_BIT_CT_SUPPORT

- **PYSODBUILDER_EXTENDED_CT_BITFIELD**
  Extended CT bit field is to be created

   Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SPDO_cfg_EXTENDED_CT_BIT_FIELD


- **PYSODBUILDER_FRAMECOPYINTERN**

  Enable or disable the copy of the received SPDO frames within the
  SPDO_ProcessRxSpdo(). If this define is enabled then the buffer of the
  received SPDO frame will be copied into a internal SPDO buffer. The buffer
  of the received SPDO frame is unchanged and the SCM UDID decoding is
  accomplished in the internal SPDO buffer. Otherwise the SCM UDID decoding is
  accomplished in the buffer of the received SPDO frame, the buffer data is
  changed.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SPDO_cfg_FRAME_CPY_INTERN

- **PYSODBUILDER_NUMLOOKUPTABLE**

  Define to enable or disable the using of the Lookup table for the SPDO
  number assignment. If the loop-up table is enabled then the SPDO filtering
  works faster but it needs always 2 Byte * 1024 memory. If the loop-up table
  is disabled then a linear search table is used to filter the SPDO frames and
  the filtering works slower because of linear searching. This linear search
  table needs 4 Bytes per SADR configured in the SPDO communication parameters.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE

- **PYSODBUILDER_CONNECTIONVALIDBITFIELD**

  Connection Valid bit field is to be created

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SPDO_cfg_CONNECTION_VALID_BIT_FIELD

- **PYSODBUILDER_CONNECTIONVALIDSTATISTIC**

  Connection Valid statistic counter field is to be created

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  \sa SPDO_cfg_CONNECTION_VALID_STATISTIC

- **PYSODBUILDER_MAXRXSYNCEDPERTX**

  Maximum number of RxSPDOs to be synchronized over a TxSPDO

  Allowed values: 1..1023
  \sa SPDO_cfg_MAX_SYNC_RX_SPDO

- **PYSODBUILDER_NOTANSWEREDTR**

  Configures the number of not answered TR (internal Time Request
  counter). If this number is reached then a time synchronization error will
  happen. (See Time Synchronization Consumer in the EPLS specification)

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..65535
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE
  \sa SPDO_cfg_NO_NOT_ANSWERED_TR

### txspdocom settings {#sect_pysodbuilder_cmakeconfig_txspdocom}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_TX_MAXMAPENTRIES | Maximum number of the Tx SPDO mapping entries | 4
PYSODBUILDER_TX_MAXPAYLOADSIZE | Maximum payload size of the Tx SPDOs in bytes | 4
PYSODBUILDER_TX_SPDOSACTIVATEDPERCALL | Number of Tx SPDOs activated per SSC_ProcessSNMTSSDOFrame() call | 1
PYSODBUILDER_TX_MAXSPDO | Maximum number of the Tx SPDOs | 1
PYSODBUILDER_TX_MAXSPDOSDG | Maximum number of the Tx SPDOs of a SDG instance | 0


- **PYSODBUILDER_TX_MAXMAPENTRIES**

  Maximum number of the Tx SPDO mapping entries

  Allowed values: 1..253
  \sa SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES

- **PYSODBUILDER_TX_MAXPAYLOADSIZE**

  Maximum payload size of the Tx SPDOs in byte

  Allowed values: 1..254

  0 is not allowed because this define is used to declare an array and arrays
  can not be declare with 0 element. To configure a Tx SPDO with payload length
  0, the number of entries configured in the Tx SPDO mapping parameter has to
  be set to 0.
  \sa SPDO_cfg_MAX_LEN_OF_TX_SPDO

- **PYSODBUILDER_TX_SPDOSACTIVATEDPERCALL**

  Tx SPDOs are activated at the transition from Pre-operational to
  Operational state. To execute this transition, SNMTS_SN_set_to_op command is
  sent. This define declares how many Tx SPDOs are activated per
  SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command
  processing. Decrementing this define reduces the execution time of
  SSC_ProcessSNMTSSDOFrame() but increases the number of
  SSC_ProcessSNMTSSDOFrame() calls.

  Allowed values: 1..SPDO_cfg_MAX_NO_TX_SPDO
  \sa SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL

- **PYSODBUILDER_TX_MAXSPDO**

  Maximum number of the Tx SPDOs.

  Allowed values: 1..1023
  \sa SPDO_cfg_MAX_NO_TX_SPDO

- **PYSODBUILDER_TX_MAXSPDOSDG**

  Maximum number of the Tx SPDOs of a SDG instance.

  Allowed values: 0 (for IO's)
  \sa SPDO_cfg_MAX_NO_TX_SPDO_SDG

### rxspdocom settings {#sect_pysodbuilder_cmakeconfig_rxspdocom}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_RX_MAXMAPENTRIES | Maximum number of the Rx SPDO mapping entries | 4
PYSODBUILDER_RX_MAXPAYLOADSIZE | Maximum payload size of the Rx SPDOs in bytes | 4
PYSODBUILDER_RX_SPDOSACTIVATEDPERCALL | Rx SPDOs are activated per SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command processing | 1
PYSODBUILDER_RX_MAXSPDO       | Maximum number of the Rx SPDOs            | 1
PYSODBUILDER_RX_MAXSPDOSDG    | Maximum number of the Rx SPDOs of a SDG instance | 0

- **PYSODBUILDER_RX_MAXMAPENTRIES**

  Maximum number of the Rx SPDO mapping entries

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..253
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE
  \sa SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES

- **PYSODBUILDER_RX_MAXPAYLOADSIZE**

  Maximum payload size of the Rx SPDOs in byte

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..254
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE
  \sa SPDO_cfg_MAX_LEN_OF_RX_SPDO

- **PYSODBUILDER_RX_SPDOSACTIVATEDPERCALL**

  Rx SPDOs are activated at the transition from Pre-operational to
  Operational state. To execute this transition, SNMTS_SN_set_to_op command is
  sent. This define declares how many Rx SPDOs are activated per
  SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command
  processing. Decrementing this define reduces the execution time of
  SSC_ProcessSNMTSSDOFrame() but increases the number of
  SSC_ProcessSNMTSSDOFrame() calls.

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..SPDO_cfg_MAX_NO_RX_SPDO
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE
  \sa SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL

- **PYSODBUILDER_RX_MAXSPDO**

  Maximum number of the Rx SPDOs.
  If the value of this define is 0 then the code size is reduced and the data
  in the received SPDOs and Time Response SPDOs will not be processed. Only the
  Time Request SPDOs will be processed.

  Allowed values: 0..1023
  \sa SPDO_cfg_MAX_NO_RX_SPDO

- **PYSODBUILDER_RX_MAXSPDOSDG**

  Maximum number of the Rx SPDOs of a SDG instance.

  Allowed values: 0 (for IO's)
  \sa SPDO_cfg_MAX_NO_RX_SPDO_SDG

### scmcfg settings {#sect_pysodbuilder_cmakeconfig_scmcfg}
- **PYSODBUILDER_MAXNODES**

  Configures the maximum number of Safety Nodes configured in the
  SADR-DVI List. For every SN, a SNMTM and a SSDOC FSM is allocated.

  Allowed values:
   - if EPLS_cfg_SCM == EPLS_k_ENABLE then 2..1023
   - if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE
  \sa SCM_cfg_MAX_NUM_OF_NODES

- **PYSODBUILDER_PROCESSEDNODESPERCALL**

  This define configures the number of processed Safety Nodes per call
  of the function SCM_Trigger().

  Allowed values:
   - if EPLS_cfg_SCM == EPLS_k_ENABLE then 1..1023
   - if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE
  \sa SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL

- **PYSODBUILDER_MAXSADR**

  This define configures the largest SADR in the SADR-DVI list and SOD index
  ( = 0xCC01 + SCM_cfg_MAX_SADR_VALUE) in the SADR-UDID-List which is read
  during the SCM initialization. Decreasing this define accelerates the
  execution of the initialization of the SCM module.

  Allowed values:
   - if EPLS_cfg_SCM == EPLS_k_ENABLE then 2..1023
   - if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE
  \sa SCM_cfg_MAX_SADR_VALUE

## Execution of the pysodbuilder tool {#sect_pysodbuilder_execution}
When a Makefile was generated with **CMake**, the execution of `make help`
lists several pysodbuilder-targets.
`make pysodbuilder` will execute the tool and create the customised .c and
header files. If necessary, it will also generate an appropriate binding file
automatically before the generation of the sources.

### Execution of the pysodbuilder tool via commandline {#sect_pysodbuilder_cli}
It is also possible to run the pysodbuilder tool from commandline with
the following possible command line switches:

  - `-h`, `--help`            show this help message and exit

  - `-d FILE`, `--osdd-file FILE`
                        osdd file to obtain values from

  - `-m MODULE_ID`, `--module-id MODULE_ID`
                        generate files for given module id. this option is
                        necessary, when using osdd files which contain several
                        modules

  - `-s FILE`, `--settings-file FILE`
                        uses the given settingsfile.

  - `-i [FILE | DIRECTORY [FILE | DIRECTORY ...]]`, `--input [FILE | DIRECTORY [FILE | DIRECTORY ...]]`
                        input files or directories containing the files used
                        for code generation, basically .h and .c files.

  - `-o DIRECTORY`, `--output DIRECTORY`
                        output directory

  - `--overwrite-input-files`
                        overwrite input files with generated files.

  - `-n ENCODING`, `--encoding ENCODING`
                        specyfiy file encoding

  - `-l`, `--list-modules`    list modules contained in specified osdd file

  - `--generate-binding-file FILE FILE`
                        use schema file (.xsd) to generate a bindings file
                        (.py).

  - `--no-overrule-osdd`    settings obtained from osdd file do not get overruled,
                        if the corresponding values in the settings file are
                        bigger.

  - `--remove-cog-comments`
                        remove comments used for code generation out of the
                        generated files. this option requires -o DIRECTORY.
                        Also, the output directory must not be part of the
                        input directories to prevent unwanted overwriting of
                        files.

  - `--dry-run`             prints the input files with the generated code, but
                        doesn't save the result

  - `-v`, `--verbose`         increase verbosity of the program

  - `--log FILE`            write log to file

  - `--version`             print version of the program


### Execution of the unit-tests {#sect_pysodbuilder_unittests}

The unittests of pysodbuilder can be executed by enabling the CMake option
**UNITTEST_TOOLS** and executing the target `tst_pysodbuilder` or the main
unittest target `test`.
