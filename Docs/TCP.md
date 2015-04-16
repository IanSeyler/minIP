
TCP
========

TCP (Transmission Control Protocol)


Anatomy of an TCP packet
--------

TCP builds upon IP so you'll see familiar IP headers here.

TCP packet layout:

	Ethernet header:
		0-5,   Destination MAC
		6-11,  Source MAC
		12-13, Type IP (0x0800)
	IP header:
		14,    Version/Header Length (Usually 0x45)
		15,    Differentiated Services Field (0x00)
		16-17, Total Length
		18-19, Identification
		20-21, Flags/Fragment Offset
		22,    Time To Live (Usually 0x40)
		23,    Protocol (0x06 for TCP)
		24-25, Header Checksum
		26-29, Source IP
		30-33, Destination IP
	TCP header:
		34-35, Source Port
		36-37, Destination Port
		38-41, Sequence Number
		42-45, Acknowledgement Number
		46-47, Data Offset/Flags
		48-49, Window Size
		50-51, Checksum
		52-53, Urgent Pointer


The TCP Handshake
------

It's as easy as 1-2-3! Really!

Client: "Hi! I'd like to connect to you. Is that ok?"

Server: "Sure, I'll allow that."

Client: "Thanks!"


		          ╔═══════════════════╗         
		          ║TCP 3-way handshake║         
		          ╚═══════════════════╝         
	                                        
		 ┌────────┐                   ┌────────┐
		 │ Client │ ──────┬───┬─────▶ │ Server │
		 └────────┘       │SYN│       └────────┘
		                  └───┘                 
		 ┌────────┐                   ┌────────┐
		 │ Client │ ◀───┬───────┬──── │ Server │
		 └────────┘     │SYN/ACK│     └────────┘
		                └───────┘               
		 ┌────────┐                   ┌────────┐
		 │ Client │ ──────┬───┬─────▶ │ Server │
		 └────────┘       │ACK│       └────────┘
		                  └───┘                 

The client wants to connect to a server and sends a SYN (SYNchronize) packet. The server then responds with a SYN-ACK (SYNchronize-ACKnowledge) packet. The client sends the server a ACK (ACKnowledge) packet.
