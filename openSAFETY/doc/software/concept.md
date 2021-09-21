openSAFETY Software Concept {#page_swconcept}
===========================

[TOC]

The main task of openSAFETY is too establish a information-secured communication network with other devices. Each device is called \ref SN. The openSAFETY specification and the provided software stack provides all necessary functions and methods to implement an SN which participates with such a network.

The following descriptions not only cover generic features of the openSAFETY specification, but also their specific implementation for the openSAFETY software stack.

# Terminology {#sect_terminology}

## Safety Domain {#sect_sd}

All participants of an openSAFETY network, which are logically linked together, are participants of the same **Safety Domain** (SD). This safety domain consists of only \ref sect_sn which either actively communicate or assume a passive role. All communication within each network is identified by a common **Safety Domain Number** (SDN), as well as the **UDID** of the main **Safety Configuration Manager** (SCM). 

It is assumed, that in a normal Safety Domain one device is the main communication partner for every other device. Therefore every device inside the network must allow for two SPDO channels, one for input and one for output, even if the device does has any output implementation. This channel pair is used for time synchronization during SPDO communication. As a result of this limitation, the maximum number of communicating devices inside a Safety Domain is 511 SNs (1023 being the biggest number configurable, and dividing this by 2).


## Safety Node {#sect_sn}

Every member of an openSAFETY network is called a **Safety Node** without exception. The main roles of each SN are defined as follows:

* Means of accepting and actively requesting a network configuration
* Means of parameterization using a pre-defined parameter set
* Means of verification of such a pre-defined parameter set
* Ability to accept cyclic and acyclic traffic
* Ability to exchange process data with other nodes
* Ability to time-synchronize with other nodes

For a detailed description of the possible states for an SN, please refer \ref sm_sn_powerup

## Safety Configuration Manager {#sect_scm}

An SCM is a normal SN which additionally assumes the role of an SCM inside the SD network. All SCM information exists additional to normal SN implementations. It is not a necessity, that the SCM will actively participate during cyclic operations of the network. 

