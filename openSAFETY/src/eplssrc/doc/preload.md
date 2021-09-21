# SSDO Up- and Download {#page_preload}

**SSDO** messages are transported in a Request/Response fashion between an
**SSDO server** and **SSDO client**. An SSDO server is defined as a SN which
implements a **Safe Object Dictionary** (SOD), and handles requests send by
the SSDO client. Those requests can either be download requests, by which the
client requests information stored in the SOD of the server, or uploads where
the server has to store the information from the client in the SOD.

Every message utilizes a request/response principle. A request is sent by the
client and answered in one of the following ways:

* As an **ABORT** where the request is invalid, or the request refers to an
invalid index/subindex combination
* As a **RESPONSE** in which the transport is acknowledged, or in the case of
a download request, the information will be transported
* With a timeout, which usually occurs, if there is an issue on the server side
in which case the client resets the connection.

The speed of this request/response messaging depends on the underlying black
channel and its capacity to transport such messages. Therefore it can lead to
longer then necessary message cycles and delays during boot-up.

##Preload implementation

With Preload, the client asks the server with the first request, if the server
has a message queue, and the size this queue has been implemented with. Upon
receiving the size, the client sends as many requests as fit in the queue,
without waiting for a response. As soon as all messages are sent the client
waits for the first message to be answered by a response, before sending the
next frame, and so forth, until all messages are being transferred.

> **Note**: As of now, only download messages support Preload.

> **Note**: The maximum number for the queue size is **15**

##Error Handling

In the case that the server responds to the first request with an error, the
client falls-back to the usual request/response rythm, not utilizing any message
queues.

If the server sends a queue size of 1, the principle in communication is
identical, as if the server does not support preload. A queue size of 0 is not
valid, and will be seen as an error in which case the client will also
fall-back to the normal request/response method.

##Acknowledgment and Retransmission

The server must respond to the request in the same order they have been
received. If the server misses a package, or he needs a package to be
retransmitted, he will acknowledge the last successfully received package number.
In such a case the client will assume, that all packages with numbers higher
then the acknowledged number have been lost and will initiate their
retransmission.

If at least one package has been received, the client will not automatically
switch back to a non-preload form or request/response.
