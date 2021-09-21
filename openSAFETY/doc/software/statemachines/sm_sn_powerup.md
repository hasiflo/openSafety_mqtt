# SN Power Up {#sm_sn_powerup}


![SN Power Up cycle](stack_sn_power_up.png)

This statemachine describes the steps each SN has during normal operations. At power up, the \ref sect_sn starts the self initialization. An SN can contain a flash memory where all data is stored. This data is loaded during the self initialization. After which, the SN enters the Pre-Operational state. In this state the SN can get parameters from the \ref sect_scm. The SN also sends cyclic messages to the SCM to inform the SCM about its status. When the SN switches from Pre-Operational to Operational, it may store all parameters within the flash memory.

* **Initialization**: This is the state immediatly after Power-On and it might be used to initialize the firmware. During this phase the normal power-up and initialization of the stack has to take place, especially \ref SSC_InitAll()

* **Pre-Operational**: During this state the openSAFETY stack already receives messages through an acyclic channel. Cyclic message may be ignored. The stack will accept any \ref SNMT and \ref SSDO messages through \ref SSC_ProcessSNMTSSDOFrame() and will react accordingly. The methods \ref SHNF_GetTxMemBlock() and \ref SHNF_MarkTxMemBlock() must already be working in this phase. All cyclic messages can be ignored. \note An SN may exist in this state indefinitely, which can be used to implement an SCM without it actually engaging in cyclic data-exchange with other SNs and therefore existing separately.

* **Operational**: Once the stack has verified all parameters it has been sent, and received a \ref SNMT_SN_status_Operational message by the SCM which included the correct Timestamp, it will switch to this state. In this state, two functions will have to be called, with \ref SPDO_ProcessRxSpdo() being the first and \ref SPDO_BuildTxSpdo() the second. The order should be kept. \attention The openSAFETY stack will leave the operational state only if it has received a \ref SN_switch_to_preoperational message from its configured SCM or it had achieved a timeout, while waiting for a response on \ref SN_reset_guarding_SCM, or it has run into a FailSafe scenario, from which it may only be revived by a power-cycle  
