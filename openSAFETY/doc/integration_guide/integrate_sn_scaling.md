Scaling the openSAFETY stack {#integrate_sn_scaling}
----------------------------

The openSAFETY stack can get scaled by changing the settings of the constants provided in the EPLScfg.h. The EPLStarget.h contains platform specific definitions.

Note: After any modification within the EPLScfg.h or EPLStarget.h the unit tests have to be adapted and run through.

All defines are explained within these headers. For a SN mainly only the defines for the number of cyclic payload bytes as well as the number of available mapping entries do have to be set to the required number. The following example defines an SN which has 1 TxSPDO and 1 RxSPDO. The defined values may differ depending on the implementation of the corresponding device.

    #define EPLS_cfg_MAX_PYLD_LEN                   8
    #define SPDO_cfg_MAX_NO_TX_SPDO                 1
    #define SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL        1
    #define SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES       8
    #define SPDO_cfg_MAX_LEN_OF_TX_SPDO             8
    #define SPDO_cfg_MAX_SYNC_RX_SPDO               1
    #define SPDO_cfg_MAX_NO_RX_SPDO                 1
    #define SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL        1
    #define SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES       4
    #define SPDO_cfg_MAX_LEN_OF_RX_SPDO             8

Typically a SN only has one RxSPDO and only one TxSPDO. The maximum payload length is 8 bytes (relevant for SNMTs and SSDOs). The number of mapping objects is equivalent to the number of variables (objects) to be transported (received or sent). The length of an SPDO is the number of payload bytes the SPDO does need to transport.