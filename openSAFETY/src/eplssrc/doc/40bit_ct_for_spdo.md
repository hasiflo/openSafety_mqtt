# 40 Bit SPDO Counter {#page_40bit_ct_for_spdo}

**openSAFETY** utilizes a consecutive timer to ensure protection against various
error scenarios, as defined by **IEC 61784-3**. With openSAFETY 1.5 this counter
is being increased to a 40-bit value to further protect against data overflow of
this counter.

Dataoverflow is critical, as modern computer networks may apply switching
techniques throughout the network, which not only route packages but also
prioritize and queue packages depending on various network metrics to better
utilize infrastructure and increase throughput times. Due to such techniques,
some packages may be queued for too long. If such packages are re-inserted
into the network at a later point, the ct values of the openSAFETY frames may
collide with a current ct value, although the network has seen some iterations
of the ct value in the meantime.´

By utilizing a 40-bit counter, the time between two counter overflows increases
drastically. CT value collisions occur much more infrequent because of it and
can be detected more easily.


## Network Bootup

openSAFETY does not dictate conditions on how a SN generates a ct value, beside
the rate by which the counter has to increase. Common sense dictates, that this
will lead to a scenario, where each SN usually starts with a CT value of 0.

This can result in an issue, where re-inserted packages recognized as valid, if
the SN has rebooted between the time the packages have been stored initially, to
the time they are being re-inserted into the network.

To ensure further protection, an openSAFETY network pre-seeds the timer values
of it's participants using a **SNMT extended service**. If this service is not
accepted by the device during boot-up, the SCM does not accept the SN as a valid
device and further stops parameterization. During the next boot-up of the
device, the SN assumes that the device is not capable of 40-bit ct values and
does not preseed the timer, thus resulting in a valid boot-up and 16-bit ct
values for all SPDOs produced and consumed by this device.


##SPDO feature set

It is not necessary, that all devices operating in the same network can utilize
40-bit ct values. Therefore the devices need to agree if 40-bit ct SPDOs are
being used for the network or not. This handshake is initiated by a producer, by
adding a flag to all produced SPDOs as soon as the producer starts sending data.
Connection validation is still set to *false* at this point. The consumer can
acknowledge this feature set during the initial time-synchronisation phase. If
this acknowledgment is successful, the producer switches to 40-bit ct SPDOs as
soon as the connection is deemed to be valid. This negotiation takes place every
time the connection is invalid and has to be switched to valid.
