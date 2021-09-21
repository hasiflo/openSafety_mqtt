openSAFETY Software Structure {#page_swstructure}
===========================

[TOC]

# openSAFETY Software Structure {#sect_swstructure}

The openSAFETY software stack is split up in separate modules. The interface to the fieldbus network is called [SHNF] and is not part of the stack software. This allows for an easy adjustment of the openSAFETY stack to different target systems. The following picture shows the openSAFETY software stack structure.

![openSAFETY software structuree](./software_structure.png)

Relationships between individual modules are indicated by arrow symbols. The modules \ref SERR, \ref SCFM and \ref EPLS are auxiliary and may be used by any other module. Modules which have an indication in the form of a black marker, provide a function interface for the target application. This interface is exported via a header file that is named after the module in question followed by api.h. For instance, the module \ref SSC provides functions that are defined in the header file SSCapi.h. These functions are described in greater detail in their respective module section and in the \ref page_integration_guide.

## openSAFETY auxiliary support module (EPLS)  {#sw_epls}

The internal module \ref EPLS is an auxiliary module with global agreements (e.g. define statements). This module only contains header files, therefore does not provide any implementations.

## Safety Control Flow Monitoring (SCFM) {#sw_scfm}

The \ref SCFM module offers functionality for monitoring correct program flow.

To monitor correct program flow, the user must read and compare the number of function calls on both safety controllers (see figure [openSAFETY Software Structure](\ref sect_swstructure) ) using the function \ref SCFM_GetResetPath(). If the values are different, then the application must react accordingly to this error.

To monitor correct program flow, the openSAFETY software calls the macro \ref SCFM_TACK_PATH(). This can also be used by the application.

## Safety Configuration Manager (SCM) {#sw_scm}

The \ref SCM module is used for configuring, verifying and monitoring all SNs within an openSAFETY domain (\ref sect_sd). The configuration can also be terminated by calling the function \ref SCM_Deactivate() (e.g. to change the configuration). The configuration can then be started from the beginning by calling \ref SCM_Activate().

The \ref SOD entries that are relevant to the configuration cannot be changed during the configuration. The function \ref SCM_Trigger() must be called cyclically to handle the configuration of any \ref SN.

The function \ref SCM_ResetNodeGuarding() can be called to restart the monitoring (Node Guarding). The callback function \ref SAPL_ScmUdidMismatchClbk() is called in the event of a "UDID mismatch". A "UDID mismatch" must be confirmed by calling the function \ref SCM_OperatorAck().

If the activation of an \ref SN fails, then the function \ref SAPL_ScmSnFailClbk() is called. The callback functions \ref SAPL_ScmNodeStatusChangedClbk() and \ref SAPL_ScmRevisionNumberClbk() inform the application if the state of a node changes or if a revision number is received.

The \ref SCM also checks, weather an \ref SN is to be processed or not using the function \ref SAPL_ScmProcessSn() which the application must provide. Using this function, the application can exclude certain SNs from being processed.

Problems could otherwise occur such as on the object 0x100C (GuardTime_U32) because it is read just once when the \ref SCM is started. Problems could also rise if the [SN] locks the [SOD] to calculate the CRC because the \ref SCM also has to access the [SOD] (e.g. to enter the node state of other SNs).

The \ref SCM always configures and monitors just one domain. This is why the \ref SCM is always assigned the fixed instance 0.

## Safety Domain Number (SDN) {#sw_sdn}

The internal module \ref SDN makes it possible to assign a Safety Domain Number to an instance number.

## Safety Error (SERR)  {#sw_serr}

The \ref SERR unit makes it possible to detect errors in the stack and to inform the application via the function \ref SAPL_SERR_SignalErrorClbk(). Additionally, the application can read a detailed error string about an error by using the function \ref SERR_GetErrorStr().

If a "Fail Safe" error occurs (see "error type"), then the application must create a safe state for the device (potentially including all relevant \ref SOD objects).

## Safety Frame Serialization (SFS) {#sw_sfs}

The internal module \ref SFS is used to group frames (serialize), to ungroup frames (deserialize) and to check frames.

## Safety Network Management Master (SNMTM)   {#sw_snmtm}

The \ref SNMTM module provides services for controlling the state of SNs as well as for monitoring and assigning an \ref sect_sn to SADRs. The module is used by the \ref sect_scm mostly for sending \ref SNMTM requests. Responses are forwarded to the \ref SNMTM via the \ref SSC unit. The request must be sent again if a response is not received.

The function \ref SNMTM_BuildRequest() must be called cyclically to resend a service request if necessary.

## Safety Network Management Slave (SNMTS) {#sw_snmts}

The following tasks are performed by the \ref SNMTS module:
* Processing \ref SNMT requests and creating a response, if necessary. When doing this, the module is called internally by the \ref SSC module
* Monitoring the guard and refresh time. To monitor the guard and refresh time, the function \ref SNMTS_TimerCheck() must be called cyclically.
* Managing the node state. After initialization, the application can optionally update the object directory (e.g. with values saved in non-volatile memory).

The function \ref SNMTS_PerformTransPreOp() must be called after initialization. When this function is called, the node is changed from the "Initialization" state to the "Pre-Operational" state (see \ref sect_sn for details)

If the callback function \ref SAPL_SNMTS_CalcParamChkSumClbk() is called, then the application must validate the \ref SOD checksum(s) and transfer the result to the function \ref SNMTS_PassParamChkSumValid().

After the callback function \ref SAPL_SNMTS_SwitchToOpReqClbk() is called, the application is then able to save parameters in non-volatile memory. Of course, the parameters can also be saved at a later point in time.

The application can call the function \ref SNMTS_GetSnState() to query the node state at any time. If the SN receives the service "SNMT_SN_ACK", then the application will be informed via the callback function \ref SAPL_SNMTS_ErrorAckClbk() that the \ref sect_scm has confirmed an error.

An openSAFETY frame will be rejected if the \ref SADR or \ref SDN does not match its own.

The services \ref SNMT_SCM_set_to_STOP and \ref SNMT_SCM_set_to_OP will be processed by the openSAFETY software stack as long as it is an \ref sect_scm. The two requests must be sent using a tool according to the openSAFETY specification.

## Safety Object Dictionary (SOD) {#sw_sod}

The \ref SOD module manages access to the object directory. One usage for the function \ref SOD_AttrGetNext() is to save the object directory in non-volatile memory because this is something that is not performed by the stack. This is necessary in order to ensure data consistency for segmented objects.

The function \ref SOD_DisableSodWrite() can be called in order to lock the entire object directory from write access (both segmented and non-segmented objects - e.g. in order to perform a CRC calculation, see \ref  SHNF). The lock can be reversed by using the function \ref SOD_EnableSodWrite().

It is especially important to make sure that write access to mandatory objects in the \ref SOD (particularly the SPDO Mapping and Communication Parameter) is not permitted in the state "Operational" or during the transition to "Operational".

The openSAFETY-SW also supports a variable length for domains and strings. Variable length means that the current length can be shorter or equal to the maximum length (for more information about this, see \ref SOD_t_ACT_LEN_PTR_DATA). The current length can be read by using the function \ref SOD_AttrGet(). The \ref SOD unit also has a few callback functions (e.g. \ref SAPL_SOD_DefaultValueSetClbk()).

Objects in the \ref SOD must be located directly in the memory (via a reference in the structure \ref SOD_t_OBJECT). This means that delayed access attempts are either not possible, or only possible via repeated reading.

## Safety Process Data (SPDO) {#sw_spdo}

The \ref SPDO module is used to cyclically exchange process data and to handle time synchronization and time validation. The function \ref SPDO_TxDataChanged() can be called if the data for an \ref SPDO has changed and the \ref SPDO should be sent even though the time has not yet expired.

The openSAFETY software supports dynamic mapping (i.e. the mapping parameters can be changed during operation). Single bits may not be mapped (i.e. one byte must always be used if a single bit is to be used). The mapping parameters can only be changed to whatever sequence needed when in the preoperational state, because mapping is not activated until the state changes to "Operational". The RxSPDO and TxSPDO mapping parameters (object 0x1800 and 0xC000 plus following objects) can also be written with 0.

When working with mapping parameters, the user must make sure that there is a valid object for the same amount of respective entries (sub-index 00h). For example, if the sub-index 00h (NumberOfEntries) equals 3, then sub-index 01h to 03h must contain valid mapping entries. The sub-indices starting with 04h are not taken into consideration, but should be 0. When mapping, the application must take into account that the maximum length is used for mapped objects with variable length (e.g. domains) in the \ref SPDO.

Due to the speed, the range of objects of an \ref SPDO is not checked and callback functions are not called.
The application must ensure that no range violations or incompatibilities occur with mapped objects. Using the function \ref SPDO_GetRxSpdoStatus() the application can additionally validate the timing of the received data itself. The RxSPDOs also do provide a connection valid bit if enabled (see \ref SPDO_cfg_CONNECTION_VALID_BIT_FIELD). The variable needs to be provided by the application but the related bits are set and reset by the stack. Each RxSPDO owns one bit in the bit field which is set if the connection to the producer is synchronized and valid.

## Safety Stack Control (SSC) {#sw_ssc}

The \ref SSC module is the central interface for the application. The openSAFETY software stack must be initialized using the function \ref SSC_InitAll() before any other API function is called. If a new \ref SNMT or \ref SSDO frame is received, then it must be transferred to the function \ref SSC_ProcessSNMTSSDOFrame().

## Safety Service Data Client (SSDOC) {#sw_ssdoc}

The module \ref SSDOC enables access to the local object directory of a remote \ref SSDO server. This can be done segmented or non-segmented and with read or write access. In addition to the segmented and non-segmented transfer a slim \ref SSDO service was added for fast downloading the initialization data to an \ref sect_sn (write access). This service may not be used by the application. The slim \ref SSDO service uses a different CRC16 (polynomial 0x755B) for which the calculation function has to be provided by the \ref SHNF.

The function \ref SSDOC_SendReadReq() can be called to read the data of an SN from the SOD. The function SSDOC_SendWriteReq() can be called to write data. Multiple simultaneous accesses are possible.

The \ref SCM also performs \ref SSDO access. The request must be repeated if a response is not received. The function \ref SSDOC_BuildRequest()  must be called cyclically to re-send requests if necessary.

## Safety Service Data Server (SSDOS) {#sw_ssdos}

The internal module \ref SSDOS enables access to the local object directory via the network. This can be done segmented or non-segmented and with read or write access. Only 1 \ref SSDO transfer can be made at one time for each instance.

Because timeout monitoring is not implemented, any current transfers will always be interrupted by a new transfer. There is only one \ref sect_scm for each \ref sect_sd. If a segmented transfer takes place, then the \ref SOD is accessed directly and therefore blocked for the amount of time needed for the transfer.
