# SCM Address Verification {#sm_scm_address_verification}


![SCM Address Verification](stack_address_verification.png)

The Safety Address Verification is used to verify the correctness of the safety network. All module changes are detected due to the uniqueness of the UDID. The object indeces 0xCC01h-0xCFFFh correspond to the possible safety addresses 1d – 1023d.

The following objects from the SOD are being used for the verification:

|Object|SOD Index|SOD SubIndex|Description|
|------|---------|------------|-----------|
|ModuleStatus|0xC400-0xC7FE|0x05|Module status for an individual SN|
|Expected UDID|0xCC01-0xCFFF|0x01-0xFE|this differs between SCM configuration modes: <ul><li> **ACM** - an already seen SN udid</li><li> **MCM** - one of currently valid defined UDIDs for this SADR</li></ul>|
|Assigned SADR|0xCC01-0xCFFF|-|Previously assigned SADR|
|Optionflag "40Bit counter for SN"|0xC400-0xC7FE|0x0C|Will result in additional SNMT message for time preeseeding|

\note If the optionflag for 40 bit counter is implemented, the necessary init value will be written to the SN > after the assignement of the SCM UDID, but before any SSDO services are being invoked.