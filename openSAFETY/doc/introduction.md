Introduction {#intro}
============
openSAFETY is defined as a bus-independent, autonomous frame, which can in principle also be inserted into standard protocols other than POWERLINK. openSAFETY has been designed so that standard data and safety data transfer is possible within the same network.

openSAFETY uses a variety of safety mechanisms. In addition to guarding data content via CRC Codes, timing of the data that is being transferred is also monitored independent of the actual (non-secure) transport protocol that is being used. This allows openSAFETY to provide secure transfer of data across non-secure networks.

Different transfer protocols and media such as CAN can also be used with openSAFETY. openSAFETY only uses the non-secure transfer layer for exchang-ing the secured openSAFETY Frames, whose contents are not interpreted by the transfer layer. Of course, POWERLINK is the preferred transfer layer because of the close relationship between the communication mechanisms in POWERLINK and openSAFETY, which enable an ideal level of performance.

A openSAFETY network can consist of up to 1023 openSAFETY nodes (so called Safety Nodes), and up to 1023 openSAFETY networks may exist in parrallel on the same fieldbus network. The openSAFETY domain can be spread across several, even inhomogenous networks. Special openSAFETY Domain Gateways enable communication between openSAFETY domains.

