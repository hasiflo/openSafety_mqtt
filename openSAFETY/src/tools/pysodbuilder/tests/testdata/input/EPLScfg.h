/*[[[cog
from pysodb.codegeneration.geneplscfgh import init
cg = init()
cg.init()
]]]*/
/* SODBuilder v0.1 generated file for "demo-sn-gpio" |generated on: 2015-03-20 10:18:13 */
/*[[[end]]]*/
#ifndef EPLSCFG_H
#define EPLSCFG_H

/*******************************************************************************
 **  constants
 **  ATTENTION : the following defines (EPLS_k_CONFIGURATION_STRING,
 **  EPLS_k_ENABLE, EPLS_k_DISABLE, EPLS_k_NOT_APPLICABLE) must not be changed.
 *******************************************************************************/
/** EPLS_k_CONFIGURATION_STRING:
    This symbol represents the EPLsafety Stack configuration as a string.*/
/*[[[cog cg.EPLS_k_CONFIGURATION_STRING() ]]]*/
#define EPLS_k_CONFIGURATION_STRING "demo-sn-gpio"
/*[[[end]]]*/


/** Basic constants for the configuration:
    These constants are used to set some configuration defines.*/
#define EPLS_k_ENABLE  1U
#define EPLS_k_DISABLE  0U

/** EPLS_k_NOT_APPLICABLE:
    This symbol is used for the definition of the defines that are not
    applicable. The value of this define must be outside of the allowed
    value range of all configuration defines. */
#define EPLS_k_NOT_APPLICABLE  2000

/*******************************************************************************
 **                    General configuration defines
 *******************************************************************************/
/*  This define configures the maximum number of instances.
    Allowed values : 1..255 */
/*[[[cog cg.EPLS_cfg_MAX_INSTANCES() ]]]*/
#define EPLS_cfg_MAX_INSTANCES 1
/*[[[end]]]*/

/*  Define to enable or disable the XXX_GetErrorStr function
    (e.g. SOD_GetErrorStr). These functions are used to print an error string
    for the error code.
    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE */
/*[[[cog cg.EPLS_cfg_ERROR_STRING() ]]]*/
#define EPLS_cfg_ERROR_STRING EPLS_k_DISABLE
/*[[[end]]]*/

/**
 * Define to enable or disable the extended telegram error statistic.
 * Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
 */
/*[[[cog cg.EPLS_cfg_ERROR_STATISTIC() ]]]*/
#define EPLS_cfg_ERROR_STATISTIC  EPLS_k_DISABLE
/*[[[end]]]*/

/*  Define to enable or disable the Safety Configuration Manager, SNMT Master
    and SSDO.
    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE */
#define EPLS_cfg_SCM  EPLS_k_DISABLE


/*  This define configures the maximum payload data length
    of a received SSDO Service Request or a transmitted SSDO Service Response on
    a SSDO server.

    **NOTE:**
    On a SSDO client the maximum payload data length of a SSDO server is
    specified in the SOD (Index 0xC400 sub-index 8 MaximumSsdoPayloadLen) and
    must be passed to the SSDO client by calling the SSDOC_SendWriteReq() and
    SSDOC_SendReadReq().

    Allowed values: 8..254

    **ATTENTION:** standard data type INT64, UINT64, REAL64
    is only available for EPLS_cfg_MAX_PYLD_LEN >= 12 */
/*[[[cog cg.EPLS_cfg_MAX_PYLD_LEN() ]]]*/
#define EPLS_cfg_MAX_PYLD_LEN  8
/*[[[end]]]*/

/*  This define configures the maximum payload data length
    of a received fast SSDO Service Request or a transmitted fast SSDO Service Response on
    a SSDO server.

    **NOTE:**
    On a SSDO client the maximum payload data length of a SSDO server is
    specified in the SOD (Index 0xC400 sub-index 8 MaximumSsdoPayloadLen) and
    must be passed to the SSDO client by calling the SSDOC_SendWriteReq() and
    SSDOC_SendReadReq(). */
#define EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO  ((EPLS_cfg_MAX_PYLD_LEN-1)*2)


/*******************************************************************************
 **    SCM configuration defines
 *******************************************************************************/
/*  This define configures the maximum number of Safety Nodes configured in the
    SADR-DVI List. For every SN, a SNMTM and a SSDOC FSM is allocated.
    Allowed values:
     if EPLS_cfg_SCM == EPLS_k_ENABLE then 2..1023
     if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE */
#define SCM_cfg_MAX_NUM_OF_NODES  EPLS_k_NOT_APPLICABLE


/*  This define configures the number of processed Safety Nodes per call
    of the function SCM_Trigger().
    Allowed values:
     if EPLS_cfg_SCM == EPLS_k_ENABLE then 1..1023
     if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE */
#define SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL  EPLS_k_NOT_APPLICABLE


/*  This define configures the largest SADR in the SADR-DVI list and SOD index
    ( = 0xCC01 + SCM_cfg_MAX_SADR_VALUE) in the SADR-UDID-List which is read
    during the SCM initialization. Decreasing this define accelerates the
    execution of the initialization of the SCM module.
    Allowed values:
     if EPLS_cfg_SCM == EPLS_k_ENABLE then 2..1023
     if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE */
#define SCM_cfg_MAX_SADR_VALUE  EPLS_k_NOT_APPLICABLE



/*******************************************************************************
 **    SPDO configuration defines
 *******************************************************************************/
/*  Define to enable or disable the copy of the received SPDO frames within the
    SPDO_ProcessRxSpdo(). If this define is enabled then the buffer of the
    received SPDO frame will be copied into a internal SPDO buffer. The buffer
    of the received SPDO frame is unchanged and the SCM UDID decoding is
    accomplished in the internal SPDO buffer. Otherwise the SCM UDID decoding is
    accomplished in the buffer of the received SPDO frame, the buffer data is
    changed.

    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE */
/*[[[cog cg.SPDO_cfg_FRAME_CPY_INTERN() ]]]*/
#define SPDO_cfg_FRAME_CPY_INTERN EPLS_k_DISABLE
/*[[[end]]]*/


/*  Define to enable or disable the using of the Lookup table for the SPDO
    number assignment. If the loop-up table is enabled then the SPDO filtering
    works faster but it needs always 2 Byte * 1024 memory. If the loop-up table
    is disabled then a linear search table is used to filter the SPDO frames and
    the filtering works slower because of linear searching. This linear search
    table needs 4 Bytes per SADR configured in the SPDO communication parameters.

    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE*/
/*[[[cog cg.SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE() ]]]*/
#define SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE EPLS_k_DISABLE
/*[[[end]]]*/


/*  Maximum number of RxSPDOs to be synchronized over a TxSPDO
    Allowed values: 1..1023*/
/*[[[cog cg.SPDO_cfg_MAX_SYNC_RX_SPDO() ]]]*/
#define SPDO_cfg_MAX_SYNC_RX_SPDO 1
/*[[[end]]]*/

/*  Connection Valid bit field is to be created
    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE*/
/*[[[cog cg.SPDO_cfg_CONNECTION_VALID_BIT_FIELD() ]]]*/
#define SPDO_cfg_CONNECTION_VALID_BIT_FIELD EPLS_k_ENABLE
/*[[[end]]]*/


/*  Connection Valid statistic counter field is to be created
    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE*/
/*[[[cog cg.SPDO_cfg_CONNECTION_VALID_STATISTIC() ]]]*/
#define SPDO_cfg_CONNECTION_VALID_STATISTIC EPLS_k_DISABLE
/*[[[end]]]*/


/*  Maximum number of the Tx SPDOs.
    Allowed values: 1..1023*/
/*[[[cog cg.SPDO_cfg_MAX_NO_TX_SPDO() ]]]*/
#define SPDO_cfg_MAX_NO_TX_SPDO 1
/*[[[end]]]*/

/*  Maximum number of the Tx SPDOs of a SDG instance.
    Allowed values: 0 (for IO's)*/
#define SPDO_cfg_MAX_NO_TX_SPDO_SDG  0

/*  Tx SPDOs are activated at the transition from Pre-operational to
    Operational state. To execute this transition, SNMTS_SN_set_to_op command is
    sent. This define declares how many Tx SPDOs are activated per
    SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command
    processing. Decrementing this define reduces the execution time of
    SSC_ProcessSNMTSSDOFrame() but increases the number of
    SSC_ProcessSNMTSSDOFrame() calls.
    allowed values: 1..SPDO_cfg_MAX_NO_TX_SPDO*/
/*[[[cog cg.SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL() ]]]*/
#define SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL  1
/*[[[end]]]*/

/*  Maximum number of the Tx SPDO mapping entries
    Allowed values: 1..253 */
/*[[[cog cg.SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES() ]]]*/
#define SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES 4
/*[[[end]]]*/

/*  Maximum payload size of the Tx SPDOs in byte
    Allowed values: 1..254
    0 is not allowed because this define is used to declare an array and arrays
    can not be declare with 0 element. To configure a Tx SPDO with payload length
    0, the number of entries configured in the Tx SPDO mapping parameter has to
    be set to 0. */
/*[[[cog cg.SPDO_cfg_MAX_LEN_OF_TX_SPDO() ]]]*/
#define SPDO_cfg_MAX_LEN_OF_TX_SPDO 4
/*[[[end]]]*/

/*  Maximum number of the Rx SPDOs.
    If the value of this define is 0 then the code size is reduced and the data
    in the received SPDOs and Time Response SPDOs will not be processed. Only the
    Time Request SPDOs will be processed.
    Allowed values: 0..1023 */
/*[[[cog cg.SPDO_cfg_MAX_NO_RX_SPDO() ]]]*/
#define SPDO_cfg_MAX_NO_RX_SPDO 1
/*[[[end]]]*/

/*  Maximum number of the Rx SPDOs of a SDG instance.
    Allowed values: 0 (for IO's)*/
#define SPDO_cfg_MAX_NO_RX_SPDO_SDG  0

/*  Rx SPDOs are activated at the transition from Pre-operational to
    Operational state. To execute this transition, SNMTS_SN_set_to_op command is
    sent. This define declares how many Rx SPDOs are activated per
    SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command
    processing. Decrementing this define reduces the execution time of
    SSC_ProcessSNMTSSDOFrame() but increases the number of
    SSC_ProcessSNMTSSDOFrame() calls.
    allowed values:
     if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..SPDO_cfg_MAX_NO_RX_SPDO
     if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE */
/*[[[cog cg.SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL() ]]]*/
#define SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL 1
/*[[[end]]]*/


/*  Maximum number of the Rx SPDO mapping entries
    Allowed values:
     if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..253
     if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE */
/*[[[cog cg.SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES() ]]]*/
#define SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES 4
/*[[[end]]]*/


/*  Maximum payload size of the Rx SPDOs in byte
    Allowed values:
    if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..254
    if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE */
/*[[[cog cg.SPDO_cfg_MAX_LEN_OF_RX_SPDO() ]]]*/
#define SPDO_cfg_MAX_LEN_OF_RX_SPDO 4
/*[[[end]]]*/


/*  This define configures the number of not answered TR (internal Time Request
    counter). If this number is reached then a time synchronization error will
    happen. (See Time Synchronization Consumer in the EPLS specification)
    Allowed values:
    if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..65535
    if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE*/
/*[[[cog cg.SPDO_cfg_NO_NOT_ANSWERED_TR() ]]]*/
#define SPDO_cfg_NO_NOT_ANSWERED_TR 100
/*[[[end]]]*/


/*******************************************************************************
 **    SOD configuration defines
 *******************************************************************************/
/*  Define to enable or disable the objects managed by the application
    Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
*/
/*[[[cog cg.SOD_cfg_APPLICATION_OBJ() ]]]*/
#define SOD_cfg_APPLICATION_OBJ EPLS_k_DISABLE
/*[[[end]]]*/


#endif
