openSAFETY Overview {#mainpage}
==============

[TOC]

# General Information {#sect_general}

openSAFETY is an IEC 61508:2010 compliant safety transport protocol suitable
for implementing safety-oriented process data exchange between individual
stations in a safety network. openSAFETY also controls the safety-oriented
parameter handling and configuration of the safety network stations.

The openSAFETY protocol stack was developed in accordance with IEC 61508:2010
of the openSAFETY protocol specification from the EPSG (
http://www.ethernet-powerlink.org).
In 2008 this stack was provided as open source solution and has thereafter been
maintained as open source project.
The openSAFETY stack is pre-certified by TÜV-Süd and TÜV-Rheinland according
to IEC61508 up to SIL3.

The openSAFETY stack is provided with a BSD license as part of the openSAFETY
distribution, which also contains the documentation of the source code
and a CRC calculation tool.
The distribution can be downloaded from SourceForge free of
charge (http://opensafety.sourceforge.net).

Also subject to the BSD license, an openSAFETY Demo
has been made available (http://opensafetydemo.sourceforge.net).
The openSAFETY Demo is an example implementation of an openSAFETY Safe Node
over POWERLINK black channel, showing the openSAFETY stack integration and
safe application interface. This allows setting up a demonstrator in a short
amount of time and gathering first experience with the technology.

For product development in accordance with IEC61508:2010 further measures and
documentation shall be presented to the responsible certification
body (TÜV, UL, ...).
The concrete measures and required documents can be
obtained from openSAFETY conformity centers and service providers.
A list of certified test centers and service providers for openSAFETY
is available from the [EPSG](http://www.ethernet-powerlink.org).

> Current version: \ref sect_cl_152_update

# Packages {#sect_packages}

There are two kinds of packages available which contain the openSAFETY stack software:
- openSAFETY Distribution

  Freely available on SourceForge (http://opensafety.sourceforge.net).

- openSAFETY Development Kit

  Product provided by service providers, containing documentation and
  tooling that decrease the certification effort.
  An overview about technology providers, tools and products can be found at
  http://www.ethernet-powerlink.org/en/products/.


## Package contents {#sect_package_contents}

| openSAFETY Development Kit                | openSAFETY Distribution                   |
| ----------------------------------------- | ----------------------------------------- |
| Stack                                     | Stack                                     |
| \ref page_tools "CRC tools (calc, check)" | \ref page_tools "CRC tools (calc, check)" |
| \ref page_tools "pysodbuilder"            | \ref page_tools "pysodbuilder"            |
| Documentation                             | Documentation                             |
| Stack unit tests                          |                                           |
| TÜV Certificate                           |                                           |
| SW Manual                                 |                                           |
| Stack Integration Requirements            |                                           |
| Specifications                            |                                           |


# How to start {#sect_how_to_start}

This software uses a CMake based build system to allow for cross-platform
distribution and development.
Please refer to \ref page_software_distribution for detailed instructions
on prerequisites and system requirements as well as
instructions for generating the documentation.

# Operation and Maintenance {#sect_operation_maintenance}

## Compatibility Information {#sect_compatibility}

The openSAFETY software shipped with this package is fully compatible with
openSAFETY 1.4 and openSAFETY 1.5.

## General Error Tracking and Reporting {#sect_error_tracking_reporting}

If an implementing / integrating entity detects an error for which the
openSAFETY software might be responsible, it must be reported to:
- info@ethernet-powerlink.org

The error reports must contain the following:
- Error description and expected behaviour
- Instructions for reproducing the error
- Information on the used platform, software and openSAFETY stack version

## Error List {#sect_errors}

There are no known safety-critical errors.

## Application Notes {#sect_application_notes}

Please refer to \ref page_application_notes for information regarding
application development.

## System Integration Notes {#sect_system_integration_notes}

Please refer to \ref page_system_integration_notes for information regarding
required device timing information which has to be provided for the device.

## Change Requests {#sect_change_requests}

Change requests may be reported to:
- info@ethernet-powerlink.org

# Rights of use {#sect_rights_of_use}

> It is hereby explicitly stated that the integrating / implementing entity
> must ensure and provide proof of error-free operation of its product
> using suitable test methods when using the openSAFETY protocol stack in
> safety-related products in accordance with IEC 61508:2010.
>
> Any errors found in the openSAFETY distribution by the integrating /
> implementing entity which have been submitted and confirmed are published on
> SourceForge in the \ref sect_errors for the openSAFETY project.
> The implementing / integrating entity is also obligated to continually
> check the error list without being explicitly prompted to do so
> and to evaluate the errors in the list
> in relation to the usage of the openSAFETY stack in its product.
>
> When downloading the openSAFETY distribution, the user agrees to the
> rights of use and conditions of the BSD license as specified.

For detailed information about relevant licenses, please refer to the section
\ref page_licenses "Licenses relevant for openSAFETY technology".

# Changelog {#sect_readme_changelog}
For a detailed changelog, please refer to the section \ref page_changelog.