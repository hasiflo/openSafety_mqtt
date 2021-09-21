# Callbacks signaling change of state  {#page_callbacks_for_statechange}

An openSAFETY [SN] supports three states, two of which are used during normal
operations of a device. The third state is the `INIT` state, which only
exists during boot-up of the device.

The following states exist during normal device operations:

* `PRE-OPERATIONAL` - During this state no [SPDO] data is exchanged, and
   the device configuration is not secured and may be changed
* `OPERATIONAL` - During this state [SPDO] data is sent, and the [SOD] is
   closed and protected against manipulation

A state change may happen because the device, either triggered by the network
or on it's own intentions, asks the stack to perform a state change. Those
state changes can also happen as a transition from a state to the same state
again, indicating a certain point in the state machine ot the state change.

The following state changes can be monitored using callbacks:

##PRE-OPERATIONAL to OPERATIONAL

This state change happens after a device has been successfully configured, and
responds to a `SN set to Operational` request with a
`SN status operational` response. This happens at the end of the boot-up
phase and signals the beginning of cyclic operations for the [SN].

The callback for implementation (SAPL_SNMTS_SwitchToOpReqClbk()) has the
following prototype declaration:

    void SAPL_SNMTS_SwitchToOpReqClbk(BYTE_B_INSTNUM);

> **Attention**: To inform the SNMTS about the API confirmation of switching
> into state OPERATIONAL the application must call the API function
> SNMTS_EnterOpState(). This API function MUST NOT be called within this
> callback function.

##OPERATIONAL to PRE-OPERATIONAL

This state change happens for multiple reasons.

1. The [SCM] has requested the state change, due to new parameters being
   downloaded
2. The openSAFETY Stack has requested the change, due to timeouts for the
   lifeguard signal
3. The firmware itself has requested the change, due to error managment inside
   the [SN].

The callback for implementation (SAPL_SNMTS_SwitchOpPreOpClbk()) has the
following prototype declaration:

    void SAPL_SNMTS_SwitchOpPreOpClbk(BYTE_B_INSTNUM);

##PRE-OPERATIONAL to PRE-OPERATIONAL

The function is called by the openSAFETY Stack (unit SNMTS) to indicate that
an [SN] state transition from PRE-OPERATIONAL to PRE-OPERATIONAL was requested
by the [SCM]. The state change usually indicates, that the device has been in
a pre-operational state, and the [SCM] decided that it needs to be reconfigured.

The callback for implementation (SAPL_SNMTS_SwitchPreOpPreOpClbk()) has the
following prototype declaration:

    void SAPL_SNMTS_SwitchPreOpPreOpClbk(BYTE_B_INSTNUM);

##OPERATIONAL to OPERATIONAL

This state change happens if the [SCM] reboots, but the device does not change
back to PRE-OPERATIONAL due to the timeouts for the [SNMT] lifeguarding signals
did not exceed. This callback allows the device to check, if the additional
parameter set has been changed, although the normal configuration has not and
therefore, if the device should request an update on the additional parameter
set.

The callback for implementation (SAPL_SNMTS_SwitchOpOpClbk()) has the following
prototype declaration:

~~~{.cpp}
t_SNMTS_SWITCH_OP_ALLOWED SAPL_SNMTS_SwitchOpOpClbk(
    BYTE_B_INSTNUM_ UINT8 * const pb_errorGroup,
    UINT8 * const pb_errorCode
);
~~~

The callback has to return one of the following values as a result. Based on
that result, a different action may be applied.

* [\ref SNMTS_k_FAIL] - The state change is denied
* [\ref SNMTS_k_BUSY] - The state change can not yet be performed, the module is
  still calculating
* [\ref SNMTS_k_OP] - The state change can be performed

~~~{.cpp}
typedef enum
{
    SNMTS_k_FAIL,
    SNMTS_k_BUSY,
    SNMTS_k_OP
} t_SNMTS_SWITCH_OP_ALLOWED;
~~~

> **Attention**: Due to the fact, that the SOD is locked during OPERATIONAL,
> the object 0x101A has been redefined as being writeable at all times, including
> during OPERATIONAL. Therefore the parameter download using 0x101A has to be
> guarded, so that write and parameter operations are checked if they are valid.
> Objects 0x2800-0x2FFF remain writable during OPERATIONAL, as defined in the
> openSAFETY specification.
