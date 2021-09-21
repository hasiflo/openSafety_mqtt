openSAFETY Device Description {#page_osdd}
================

[TOC]

# General Information {#sect_osdd_general}

openSAFETY is defined as a fieldbus-independant machine safety protocol. Therefore the description of such devices may happen separately from the definition of the black-channel transport for safety information. openSAFETY need not to be altered, if only the fieldbus communication interface is exchanged.

To ensure, that such a device may be described in a fashion, that will be transportable across mulitple fieldbus definitions, a separate decsription file is needed. This description file is called OSDD.

# Main structure {#sect_osdd_structure}

The main structure of an OSDD file is described in this example:

~~~{.xml}
<?xml version="1.0" encoding="UTF-8"?>
<osdd
   xmlns="http://www.ethernet-powerlink.org/osdd"
   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
   xmlns:xi="http://www.w3.org/2001/XInclude"
   Version="1.5">
    <Module id="GenericSafeIO" Name="GenericSafeIO" openSafetyRelease="1.5" isSCM="false"  DefaultCRC32="0x65187264" >
        <Description>Safe Digital IO Module , 8 Inputs, 2 Outputs</Description>
        <ModuleFunctions> … </ModuleFunctions>
        <Languages> … </Languages>
        <ModuleDefinition> … </ModuleDefinition>
        <DesignConfiguration> … </DesignConfiguration>
        <ChannelConfiguration> … </ChannelConfiguration>
        <ManufacturerSpecificObjectDictionary> … </ManufacturerSpecificObjectDictionary>
    </Module>
</osdd>
~~~

An OSDD file allways starts with a **Module** definition, in which the module and it's name is described, as well as the minimum OSDD specification version is defined. Also the main CRC is referenced, which must be verified, before the module will be accepted by an design tool.

# OSDD during device development {#sect_osdd_development}

Any openSAFETY device implements a \ref SOD as well as \link EPLScfg.h EPLScfg.h \endlink file, for scaling the openSAFETY stack as well as providing the data interface between the device firmware and the openSAFETY stack itself. Using the tool \ref pysodbuilder, both parts of the stack implementation can be generated using an OSDD file, which will ensure, that the OSDD and the device firmware will allways correlate and the device may be used correctly by the end-customer.

For more information on how to include this tool in your toolchain, please see \ref pysodbuilder.

