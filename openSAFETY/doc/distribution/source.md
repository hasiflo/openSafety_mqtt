Source Distribution {#page_source}
===================

[TOC]

# Provided Sourcecode {#sect_sourcecode}

    .\src\contrib
    .\src\eplssrc
    .\src\tools
    .\src\unittest
    .\CMakeLists.txt
    .\CMakeOptions.txt
    .\config.h.in
    .\CTestCustom.cmake

* **src\\contrib** - Includes the unittest backend (cunit), a pthread implementation for Windows plattforms and a compatibility layer library for Windows.
* **src\\eplssrc** - Includes the openSAFETY stack. Please refer to \ref page_stack for more details.
* **src\\tools** - These are the provided libraries and tools for the openSAFETY distribution. Please refer to \ref page_tools for more details
* **src\\unittest** - These are the provided unittests for the tools and for the openSAFETY stack (openSAFETY Development Kit only).
> **openSAFETY Development Kit:**
>
> Depending on the version of the openSAFETY Development Kit,
> the openSAFETY stack unittest directory (`src\unittest\eplssrc`) **may**
> contain an archive with contributed unittest adaptions
> (`contributed_unittest_adaptions.zip`).
>
> These are provided 'as is' and may be useful in case unittest changes are
> required due to different configurations or limitations on the device
> (e.g. available memory).
> For using the adaptions, overwrite the unittest directory with the archive
> content.
>
> For the openSAFETY Conformance Assessment, **all changes to the unittests
> have to be presented to and acknowledged by the Conformance Assessment Center**,
> regardless the changes were induced by usage of the contributed
> adaptions or made by the integrating / implementing entity.

**CMakeLists.txt** and **CMakeOptions.txt** contain the main script for the CMake build system as well as user-defined options for the build. The CMake system uses those options to provide a **config.h** file, generated from the **config.h.in** template found in the src directory.

# CMake Build Options {#sect_options}

|Option|Depends on|Description|Needed for pre-certification checks|Default|
|-------|-------|------|:-----:|:-----:|
|BUILD_OPENSAFETY_TOOLS|-|Builds the openSAFETY main toolchain|Yes|ON|
|BUILD_OPENSAFETY_LIBS|-|Builds the openSAFETY libraries|Yes|ON|
|BUILD_SHARED_LIBS|-|Build all libraries as shared|Yes (must be ON)|ON|
|BUILD_OPENSAFETY_DOCUMENTATION|-|Enable build target for documentation|No|ON|
|BUILD_OPENSAFETY_PYTHON_TOOLS|-|Enable pysodbuilder tool|No|OFF|
|UNITTEST_ENABLE|-|Enables the unittest build system|No|ON|
|UNITTEST_TOOLS|-|Enable unittests for additional tools|No|OFF|
|UNITTEST_XML_REPORTS|UNITTEST_ENABLE|Generate XML Reports instead of stdout|No|ON|
|CRC_ROTATED_ENABLE|BUILD_OPENSAFETY_TOOLS|Use rotated polynomials for CRC16 lookup tables|No|OFF|
|CHECKSUM_SRC_ENABLE_CHECKS|-|Enable all tools for \ref sect_precertification|Yes|ON|
|CHECKSUM_SRC_PERFORM_CHECKS|CHECKSUM_SRC_ENABLE_CHECKS|Perform \ref sect_precertification checks for the stack|Yes|ON|
|CHECKSUM_SRC_SOURCECODE|CHECKSUM_SRC_PERFORM_CHECKS|Perform \ref sect_precertification checks on the openSAFETY stack sourcecode|Yes|ON|
|CHECKSUM_SRC_WARN_ONLY_ON_FAIL|CHECKSUM_SRC_ENABLE_CHECKS|Do not fail build if stack file has been altered|No (must be OFF for actual tests)|ON|

## Additional CMake Build Options (openSAFETY Development Kit) {#sect_options_devkit}

|Option|Depends on|Description|Needed for pre-certification checks|Default|
|-------|-------|------|:-----:|:-----:|
|UNITTEST_STACK|-|Enable the unittests for the openSAFETY stack|No|OFF|
|UNITTEST_SMALL_TARGETS|UNITTEST_ENABLE|Split build targets into smaller binaries|No|OFF|
|UNITTEST_RUN_ONYLUSERDEFINED|UNITTEST_ENABLE|Only use the user-defined EPLScfg.h|No|ON|
|UNITTEST_RUN_EPLSCFG|UNITTEST_ENABLE|Provide a path for a EPLScfg.h to run the test with|No|-|
|CHECKSUM_SRC_UNITTEST|CHECKSUM_SRC_PERFORM_CHECKS|Perform \ref sect_precertification checks on the openSAFETY unittest sourcecode|Yes|ON|


# Licenses {#sect_licenses}

This contains the openSAFETY stack sources, as well as acompanying libraries and tools and their respective unittests. All software is either distributed using the BSD license, or a GPLv2 license where applicable (see \ref page_licenses for details).


