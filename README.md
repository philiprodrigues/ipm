# ipm
Inter-Process Messaging.

_The following is taken from an email Phil sent John, Eric and Kurt on September 1, 2020:_

Message-passing library requirements:
-------------------------------------

Requirements from Giovanna:

* A message is a given number of bytes that's sent to/received from a connection
* Two messaging patterns are required:
    1. Publish/subscribe, in which messages are sent by the publisher without regard for what may be listening. There may be any number of subscribers for each publisher. Problems with subscribers do not affect the publisher
    2. Direct messaging, in which there is a 1:! connection between two ends
* Name lookup, like "What is the address of the DFO in my partition?" will be provided by CCM, and is out of scope of the message passing itself
* The message passing library must be able to handle event data (tens-of-MB messages) and the corresponding data rates, if we decide to use a "traditional" approach to event building
* Messages can come from a single contiguous buffer, or multiple buffers (ie scatter/gather)
* ERS will be used for error reporting

This would be implemented as a thin wrapper around an existing messaging library, such as zeromq or boost::asio, in such a way that we would be able to change the backend in future if that becomes necessary.
