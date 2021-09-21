System structure {#concept_nwstructure}
--------------------------------

![nw_struct]

Communication data must be compared in order to ensure that the same input data has been received and identically processed with both safety controllers (SC). The stack will always generate both sub-frames of the openSAFETY frame.

The above image shows the principle system structure of an openSAFETY network node, consisting of the safety-I/O system, the two safety controllers (SC) and the communication controller (CC) with link to the non-secure network, whereby only the external interfaces of the controllers (1), (2) and (3) are displayed. 

The external interfaces of the stack are described in the chapter [INTEGRATION_GUIDE]. The external interfaces of the openSAFETY stack are described in chapter [SHNF].

Safety controllers receive openSAFETY Frames from the communication controller via a fieldbus interface (1). The safety controllers 1 and 2 make both sub-frames 1 and 2 available on their respective interfaces (2) for transmission to the communication controller. The interface (3) is used to exchange safety-related data for comparison with the other respective safety controller.


[SCM]: @ref SNMT "Safety Configuration Manager (SCM)"
[SN]: @ref SN "Safety Node (SN)"
[SDG]: @ref SDG "Safety Domain Gateway (SDG)"
[SNMT]: @ref SNMT "Safety Network Management (SNMT)"
[SSDO]: @ref SSDO "Safety Service Data Object (SSDO)"
[SPDO]: @ref SPDO "Safety Process Data Object (SPDO)"
[SERR]: @ref SERR "Safety Error Reporting (SERR)"

[SOD]: @ref SOD "Safety Object Dictionary (SOD)"

[SCFM]: @ref SCFM "Safety Control Flow Monitoring (SCFM)"
[SHNF]: @ref SHNF "Safety Hardware Near Firmware (SHNF)"

[nw_struct]: node_comm_structure.png "System structure openSAFETY node"

[INTEGRATION_GUIDE]: @ref integration_guide "Integration Guide"