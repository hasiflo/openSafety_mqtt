Sytem Integration Notes {#page_system_integration_notes}
==================

[TOC]

This page provides information to consider for the system integration of the
device with safety related design tools.

# Safe data duration {#sect_data_duration}

The safe data duration is the maximum allowed time an openSAFETY SPDO may take
for transmission from the output of the openSAFETY stack of one device
to the processing of the SPDO on the receiving device.
This value may be needed by safety related design tools and can be affected
by e.g. network cycle times, device latencies, etc.


# Intra decvice transport latency [µs] {#sect_intra_device_transport_latency}

The intra decvice transport latency is the time between the openSAFETY stack's
output of a TxSPDO frame and the frame's availability in the transmit buffer
of the network / fieldbus communication part.
In the receiving direction, it is the time between the reception of an
openSAFETY RxSPDO from the network until it is passed to the related openSAFETY
stack processing function.
The intra decvice transport latency has the unit microseconds [µs] and
can be affected by e.g. module cycle times, communication cycle times between
safety part and network part, backplane bus characteristics,
number of modules on a backlpane bus, position of the module,
fieldbus cyle time, etc.

> A manufacturer of a device is required to provide the value or calculation
> methods for the intra device transport latency of his device in e.g.
> the respective documentation.
> These methods must qualify to generate a valid data duration value,
> which can be used for calculating safe reaction time values for such devices.

