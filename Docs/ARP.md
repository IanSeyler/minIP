
ARP
========

ARP (Address Resolution Protocol) is used to find out what hardware addresses resolves to a specific IP address.

Let's say you have two computers:

	Computer A
	IP: 10.0.0.10
	HW: 0B4D23C20341

	Computer B
	IP: 10.0.0.20
	HW: 0C34A3B98012

Computer A wants to connect to computer B via its IP address but in order to send an Ethernet packet it needs to know what the hardware address of computer B is.

ARP to the rescue!

Computer A first has to send a broadcast asking the network for the hardware address.

Anatomy of an ARP request/reply
--------

ARP request packet layout:

	Ethernet header:
		0-5,   Broadcast MAC (0xFFFFFFFFFFFF)
		6-11,  Source MAC (The requestor)
		12-13, Type ARP (0x0806)
	ARP header:
		14-15, Hardware type (0x0001 Ethernet)
		16-17, Protocol type (0x0800 IP)
		18,    Hardware size (0x06)
		19,    Protocol size (0x04)
		20-21, Opcode (0x0001 Request)
		22-27, Sender MAC (The requestor)
		28-31, Sender IP (The requestor)
		32-37, Target MAC (0x000000000000)
		38-41, Target IP (The query)

ARP reply packet layout:

	Ethernet header:
		0-5,   Destination MAC (The requestor)
		6-11,  Source MAC (The replier)
		12-13, Type ARP (0x0806)
	ARP header:
		14-15, Hardware type (0x0001 Ethernet)
		16-17, Protocol type (0x0800 IP)
		18,    Hardware size (0x06)
		19,    Protocol size (0x04)
		20-21, Opcode (0x0002 Reply)
		22-27, Sender MAC (The replier)
		28-31, Sender IP (The replier)
		32-37, Target MAC (The requestor)
		38-41, Target IP (The requestor)

ARP Caching
--------

The operating system maintains a cache of ARP responses so it will not need to send out a request for the MAC of a device it has already been in contact with. On Windows and Linux you can see this cache by executing the following command:

	arp -a

ARP Spoofing
--------

Yes, it is also possible to cause network issues by sending bogus ARP replies. This is generally frowned upon by network admins.
