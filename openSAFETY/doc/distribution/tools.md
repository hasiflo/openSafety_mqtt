Tools & Libraries {#page_tools}
================

[TOC]

# openSAFETY Tools & Libraries {#sect_opensafety_toolslibs}

## openSAFETY CRC routines {#sect_oschecksum}

openSAFETY uses various routines for calculating CRC checksums during
configuration, parameterization as well as SPDO data transfer. Those routines
are documented in the specification for openSAFETY. To ease development, they
are also provided as example implementations.
See \ref oschecksum for details.

## openSAFETY CRC check utility {#sect_oscrccheck}

Build process helper tool for ensuring the CRC checksum on each stack file.

The oscrccheck utility may be used in one of two ways for
checking a file crc against a pre-defined checksum file and for calculating
the crc of a given file
It is being used to ensure the pre-certification of stack sources, where the
checksum of a given stack source is checked against a pre-calculated value.
See \ref oscrccheck for details.

## pysodbuilder {#sect_pysodbuilder}

The \subpage page_pysodbuilder tool allows the generation of customised .c and
header files which adhere to a given OSDD file.
This approach ensures consistency throughout the development
process, when developing an openSAFETY safe node.
These generated files together with the openSAFETY stack source files and
module specific sourcefiles form the firmware of an openSAFETY safe node.
See \subpage page_pysodbuilder for the user guide.