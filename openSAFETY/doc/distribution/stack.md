openSAFETY Stack {#page_stack}
================

[TOC]

# openSAFETY Software design {#sect_opensafety_design}

Any openSAFETY device uses a \ref sect_sn as the base for all communications within a single openSAFETY network. All devices must implement the necessary functions fo assuming the role of an SN in a single \ref sect_sd. One communication partner within this network may assume an additional role of \ref sect_scm to distribute parameters and configure the network as such.

The openSAFETY stack distribution provides both parts, but devices, which will not act as an SCM, may only use all the sources required for an SN, and ignore all sources and tests in regard to SCM parts completely.

For a more detailed explanation of the openSAFETY network stack, please refer to the \ref page_swconcept, or the \ref page_integration_guide

# Pre-Certification {#sect_precertification}

The openSAFETY stack is pre-certified. Therefore the source-code must not be altered, if the generated device should be certified by a certification body, or otherwise the stack has to be certified as well. To ensure, that such an alteration has not taken place, openSAFETY provides a clean method for checking if the stack and unittest sources have been altered.

From the source-code directory in the build subdirectory, execute the following with a fresh install of the **openSAFETY Development Kit** sourcecode:

> For **openSAFETY Distribution** only the stack source code will be checked.

~~~{.sh}
cd build
cmake -G "Unix Makefiles" -DCHECKSUM_SRC_ENABLE_CHECKS=ON -DCHECKSUM_SRC_PERFORM_CHECKS=ON ../src
make help
~~~

This will display a list of available build commands, among them are the following targets:

    ... CRC_CHECK_SCM
    ... CRC_CHECK_SCM_PREP
    ... CRC_CHECK_SN
    ... CRC_CHECK_SN_PREP
    ... CRC_CHECK_STACK

The source code can be checked by calling either

~~~{.sh}
make CRC_CHECK_STACK
~~~

or

~~~{.sh}
make all
~~~

in which case will also build all libraries and unittests.

## Report for openSAFETY Conformance Assessment {#sect_precert_confromance_assessment}

For openSAFETY Conformance Assessment, see \ref sect_options for required flags
for the required pre-certification check report.

In short, execute the following to perform the pre-certification checks.

From the source-code directory in the build subdirectory, with a fresh install of the **openSAFETY Development Kit** sourcecode, type:

~~~{.sh}
cd build
cmake -G "Unix Makefiles" -DCHECKSUM_SRC_ENABLE_CHECKS=ON -DCHECKSUM_SRC_PERFORM_CHECKS=ON -DCHECK_CRC_WARN_ONLY_ON_FAIL=OFF ../src
make CRC_CHECK_STACK
~~~

> Keep the output of the build. It serves as report at the openSAFETY Conformance Assessment.

