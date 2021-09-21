# Dynamic SCT {#page_dynamic_sct}

##Static SCT calculation (until openSAFETY 1.4)

openSAFETY as a protocol does not safe-guard the transported data, but rather
secures the transport-channel the data is transported on. The transported data
are process-images for a certain point in time (ProdCT). Because openSAFETY
uses a Producer-Consumer principle, the consumer checks this time against an
expected timeframe and determines, if the produced process image was generated
inside the correct time-frame and if the time expectation is met on the consumer
side. The window for the time expectation on the consumer side is called
SCT window.

To secure the transport channel, the consumer must check the transport time for
a single message between the consumer and the producer to correctly predict,
considering above principle, if the produced message was sent in the correct
time frame, and therefore the channel is still secure. This process is called
time-synchronization and must be repeated periodically during the OPERATIONAL
phase of a Safety Node (SN).

Up until openSAFETY 1.4, this time synchronization process, as well as the SCT
synchronization process relied on a pre-calculated set of timing windows. Those
calculations where based on the underlying application and a predefined
definition of the network. Therefore real-life network situations, such as
delays, packet-loss or varying jitter times, had to be caught using propagation
windows. The direct result of such a window is a greater emphasize on tolerance
factors, therefore leading to a smaller SCT window.

##Dynamic SCT calculation (beginning with openSAFETY 1.5)

With openSAFETY 1.5 **dynamic SCT** has been introduced. Dynamic SCT utilizes on
the fact, that the current time synchronization is also a measurement for the
message transit time of the network between the consumer and the producer.
Therefore the measured delay can be used to define the minimum propagation delay
of a message between the producer and the consumer. To ease calculation, it is
additionally assumed that the transfer of the request from the consumer to the
producer is imminent and the measurement only defines the time for the message
transfer between the producer and a consumer.

The new SCT windows starts at a certain point in time of a Safe Reactiontime (SRT)
window for which the measurement concludes a message should have been received
if sent synchronized with the start of the SRT window (minimum propagation delay).
Also due to the fact that it can now be assumed, that as long as the message
hits within the SRT window it must have been transmitted in the right time-frame,
the SCT window widens up to the extent of the SRT window.

The result of this approeach has two applications:

1. Utilizing the same maximum SRT as before, a much larger SCT window can be
achieved, therefore leading to a much more stable transport-channel, as the time
in which valid packages may be received, increases.
2. The new approeach does not need to be pre-calculated in a design-tool as it
relies on measurement rather then pre-defined assumptions. Therefore the
parameters, which have to be set by a design-tool are fewer, resulting in an
easier handling of timing calculations inside a design-tool.

As a consequence of 1., if the same stability and windows as before can be
applied to the underlying application, the SRT window can be shortend, leading
to faster SRT times, and this can lead to smaller overall worst case
response times.

> **Attention**: The calculation of the design-tool still has to take quartz-
> tolerance factors under consideration, to allow for a minimum drift factor
> correction in the overall calculation.
