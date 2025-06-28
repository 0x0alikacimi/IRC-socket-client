# revents & POLLIN :
	checks if the POLLIN flag is set in revents
	POLLIN : means "There is data to read" or "The socket is ready for reading."
			For a listening socket, POLLIN means new connection(s) are waiting to be accepted.



On the listening socket
	POLLIN means: There is at least one new client connection waiting to be accepted.

On a client socket
	POLLIN means: There is actual data from the client ready to be read.


The listening socket accepts connections.
The client sockets exchange data.

poll() just says "this fd is ready to read," but your program must know what that actually means for each fd
