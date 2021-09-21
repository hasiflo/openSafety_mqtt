Recommended implementations {#integrate_sn_callbacks}
---------------------------------

Implementing a function body for some of these functions is required, but the function body can be set to a default value.

- SNMTS_TimerCheck()  Needs to be called to check for a guarding timeout  

- SNMTS_GetSnState()  Used to get the actual state of the SN  

- SNMTS_PassParamChkSumValid()  To be called after the [SOD](@ref SOD) checksum calculation to tell the openSAFETY stack whether the [SOD](@ref SOD) checksum is valid or not 

- SNMTS_EnterOpState()  Used to tell the stack if it may switch to operational or not (and if not the reason why not)  

- SNMTS_PerformTransPreOp()  Used to force the openSAFETY stack to preoperational 

- SSC_InitAll()  This function will be called by the openSAFETY stack, after initialization of the stack is finished.  

- SSC_ProcessSNMTSSDOFrame()  To be called if an [SSDO](@ref SSDO) or [SNMT](@ref SNMT) is received from the network 

- SCFM_GetResetPath()  Used to get the actual program flow counter 

- SCFM_TACK_PATH()  Can be used to increase the actual program flow counter (not necessary).

- SPDO_BuildTxSpdo()  Called by the application to generate [SPDO](@ref SPDO) 

- SPDO_TxDataChanged()  Called by the application to ensure that the [SPDO](@ref SPDO) are generated within this module cycle  

- SPDO_ProcessRxSpdo()  To be called for each received [SPDO](@ref SPDO)  

- SPDO_CheckRxTimeout()  To be called after the [SPDO](@ref SPDO) for this module cycle are received  

- SPDO_GetRxSpdoStatus()  Detailed status of a Rx[SPDO](@ref SPDO) 

