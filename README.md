# networks_assign_1_2

 Information about the assignments

-Assignment 1. 
Client using customized protocol on top of UDP protocol for sending information to the server

Data Packet format can be found in the assignments_promts directory 

-Server sends back the appropriate packet (ACK or Reject) with the appropriate error sub code:
	Packet not in the right sequence
	Length miss-match of the payload
	End of packet identifier error
	Duplicate packet

Client starts a timer (ack_timer) for every packet sent to the server. If there is no response from the server, client needs to retransmit the packet ( avoiding duplicate reject packets from the server). Client retransmits the packet 3 times at most, if there is no response. If there is no ACK from the server, the client generates  “Server doesn’t respond” and stops sending the packet.


-Assignment 2. 
Client using customized protocol on top of UDP protocol, for requesting identification from server for access to the network

Communication between server and client uses ACK_timer, as described in the first part. 

Client can be imagined as a software module that is acting in behalf of a cell phone

Packets’ format can be seen in the assignments_promts directory 

Server opens and read a file, where there is information about the users( subscriber’s number, tech, paid or not)

-Server sends back packet with the corresponding sub codes, about the user in the specified format
	Subscriber doesn’t exist
	Subscriber granted access to the network
	Subscriber has paid


*For details, see the assignments_promts directory, where requirements can be found


- Instructions for compiling and running the programs

Both assignments have been implemented in OS 10.14.4 (18E226) with gcc compiler. 


-Compile and Run instructions:

Run Server First:

Compile:
cc server.c -o server

Run:
./server 3000

Then Run Client in a second terminal:

Compile:
cc client.c -o client

Run:
./client localhost 3000

