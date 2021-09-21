Implement and integrate an SOD {#integrate_sn_sod}
---------------------------------

The SOD is used to access and represent variables in the openSAFETY network.

The SOD is divided into 4 areas as can be seen in the [2] openSAFETY specification. Index 0x1000 – 0x1FFF is reserved for the communication objects. 0x2000 – 0x5FFF is reserved for the manufacturer specific objects, 0x6000 – 0xBFFF is reserved for application objects and from 0xC000 on to 0xFFFF the rest of the communication objects as well as the SCM objects do follow.

All objects which are required by the specification have to be implemented in the SN SOD.

Objects being hosted by the SOD can be accessed by the application either using the variable itself or the index/subindex of the SOD using the SOD access functions.


- SOD_AttrGet()   Returns the attributes and the entry handle of an SOD entry

- SOD_AttrGetNext()   Returns the attributes of the next SOD entry. Used for calculating the SOD CRC. 
    - Get the attribute of the first SOD entry and read it, 
    - then get the next entry and read it, 
    - get the next entry, ...

- SOD_Read()   Read an SOD entry

- SOD_Write()   Write an SOD entry

- SOD_Lock()   Locks the SOD for exclusive access

- SOD_Unlock()   Unlocks the SOD

- SOD_EnableSodWrite()   Enable write access to the SOD after the SOD is write protected in operational mode

- SOD_DisableSodWrite()   Disable write access to the SOD after being enabled

- SOD_ActualLenSet()   Returns the length of a DOMAIN parameter

- SOD_ActualLenGet()   Sets the length of a DOMAIN parameter


### Customizing the communication objects

This allows for example to refer directly to a variable within the application containing the vendor-id. Another use-case is the setting of variables after the openSAFETY stack was initialized.

Adding or removing objects in the communication object area (0x1000 - 0x1FFF) is just necessary to ensure that the SOD does contain the right amount of SPDOs and mapping entries for the SPDO.

In the above figure an example for a RxSPDO with 4 mapping entries can be seen. The number of mapping entries in the SPDO must match the corresponding define in the EPLScfg.h.

In case there are two RxSPDO the objects 0x1401 and 0x1801 need to get created. For TxSPDOs it is similar.


### Creating manufacturer specific objects

Manufacturer specific objects are objects which are device specific and are not part of an official device profile. The Objects 0x2800 – 0x2FFF are reserved for objects being able to get written in operational state and therefore may not be part of the SOD CRC. All other objects may get used for parameters like filter values or channel settings e.g. grouping two input channels equivalent or antivalent.

Those objects may be part of the parameter domain being downloaded by the SCM.


### Creating application specific objects

Application objects are commonly used for the data objects (channels) of the device. At the current state the openSAFETY stack does not have a native support for any device profile. However channels should get created using objects according to the respective device profile (e.g. 401 for IO channels).

The above example shows 3 input channels which can be transported by a TxSPDO. As can be seen those channel objects are read only (RO) and mappable (PDO). All channels (inputs and outputs) which are to be transported via SPDO have to have the PDO attribute in order to be mappable.


### Creating objects with an access callback

Objects which are being accessed by SOD access functions can have a callback set in order to inform the application about the access. This for example is to be done for the 0x101A object which is used to transfer all parameters from the SCM to the SN. After the transfer is complete, the related callback function is called by the openSAFETY stack and that way the application is triggered to parse the received parameter string.

SOD callbacks can be activated for:
- before write [SOD_k_ATTR_BEF_WR()](@ref SOD_k_ATTR_BEF_WR)
- after write [SOD_k_ATTR_AFT_WR()](@ref SOD_k_ATTR_AFT_WR)
- before read [SOD_k_ATTR_BEF_RD()](@ref SOD_k_ATTR_BEF_RD)


### Objects hosted by the application

In order that application objects can be added they have to be enabled first (see EPLScfg.h). Here the application does have to supply all functions which are needed to represent objects in the SOD.

- [SAPL_SOD_DefaultValueSetClbk()](@ref SAPL_SOD_DefaultValueSetClbk)
- [SAPL_SOD_AttrGetClbk()](@ref SAPL_SOD_AttrGetClbk)
- [SAPL_SOD_ReadClbk()](@ref SAPL_SOD_ReadClbk)
- [SAPL_SOD_WriteClbk()](@ref SAPL_SOD_WriteClbk)
- [SAPL_SOD_LockClbk()](@ref SAPL_SOD_LockClbk)
- [SAPL_SOD_UnlockClbk()](@ref SAPL_SOD_UnlockClbk)
- [SAPL_SOD_ActualLenSet()](@ref SAPL_SOD_ActualLenSet)
- [SAPL_SOD_ActualLenGet()](@ref SAPL_SOD_ActualLenGet)

If an object is not found in the normal SOD and application objects are enabled the openSAFETY stack will search the object among the application objects.