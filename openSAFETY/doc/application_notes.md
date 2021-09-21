Application Notes {#page_application_notes}
==================

[TOC]

This page provides information to consider during application development.

# Virtual SOD {#sect_virt_sod}

A virtual SOD is an application level abstraction of the SOD which increases
the overall efficiency of the SN's application, when using big SODs (complex
SN) with many equal objects / object entries.

A virtual SOD has to be provided / implemented in any case.
When not using this feature, it may be implemented in a minimum variant
according to the following code snippet.


        SOD_t_ENTRY_VIRT SAPL_s_SOD_VIRT_INST =
        {
            {0, EPLS_k_BOOLEAN, 0x1UL, NULL}, SOD_k_END_OF_THE_OD, SOD_k_END_OF_THE_OD, 0xFF, 0xFF, NULL, NULL, (SOD_t_CLBK)0
        };


        /* dummy for virtual SOD */
        const SOD_t_ENTRY_VIRT *const SAPL_ps_SOD_VIRT[EPLS_cfg_MAX_INSTANCES] =
        {
            &SAPL_s_SOD_VIRT_INST
        };

# SOD CRC Calculation {#sect_sod_crc}

> The following information depends on the design tool and SCM used.

The calculation of CRC of the SOD is necessary to check if a received parameter
set is valid. An openSAFETY SN therefore checks if the received CRC checksum
and the calculated one are equal.

The CRC checksum has to be calculated for all SOD entries, which are marked
as CRC relevant by iterating over the SOD.

In a typical scenario the following objects are CRC relevant:

- 0x100C/0x01
- 0x100C/0x02
- 0x1200/0x03
- 0x1400/0x01-0x0C
- 0x1800 as a whole
- 0x1C00/0x01-0x03
- 0x2000
- 0x2001
- 0xC000 as a whole

Incorrect values of 0x1200/0x01 and 0x1200/0x02 will prevent communication
and parameter download.
The SCM UDID is application specific, therefore
0x1200/0x04 must be excluded of SOD CRC calculation.

Also check the following:
- Correct length information of SOD entries in relation to linked data
  structures, especially domain objects
- Number of SOD mapping entries = value of CRC_Number_of_Maps_per_Spdo in device
  description = values of the defines SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES and
  SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES in stack configuration file EPLScfg.h

