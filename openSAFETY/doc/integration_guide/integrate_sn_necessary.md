Implement all necessary functions {#integrate_sn_necessary}
---------------------------------

- SAPL_SERR_SignalErrorClbk()  Errors are forwarded to this function which enables the openSAFETY stack reporting errors 

- SAPL_SNMTS_CalcParamChkSumClbk()  Calling this function the openSAFETY stack triggers the calculation of the [SOD](@ref SOD) checksum 

- SAPL_SNMTS_SwitchToOpReqClbk()  Calling this function the openSAFETY stack requests from the SN switching to operational 

- SAPL_SNMTS_ErrorAckClbk()  Calling this function the openSAFETY stack reports a received SN Acknowledge 

- SAPL_SNMTS_ParameterSetProcessed()  Calling this function the openSAFETY stack checks if the processing of the received parameters is finished 

- SHNF_GetTxMemBlock()  The openSAFETY stack calls this function to allocate memory for the generation of a safety frame 

- SHNF_MarkTxMemBlock()  The openSAFETY stack uses this function to mark an allocated memory as used 

- [HNFiff_Crc8CalcSwp()](@ref SHNF_Crc8Calc)  Used by the openSAFETY stack to calculate the openSAFETY CRC 8  

- [HNFiff_Crc16CalcSwp()](@ref SHNF_Crc16Calc)  Used by the openSAFETY stack to calculate the openSAFETY CRC 16 

- [HNFiff_Crc16_755B_CalcSwp()](@ref SHNF_Crc16CalcSlim)  Used by the openSAFETY stack to calculate the openSAFETY CRC16 for configuration data 

