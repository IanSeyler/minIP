
UDP
========

UDP (User Datagram Protocol) also known as [RFC 768](http://tools.ietf.org/html/rfc768).


Anatomy of an UDP packet
--------

UDP builds upon IP so you'll see familiar IP headers here.

UDP packet layout:

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
		23,    Protocol (0x11 for UDP)
		24-25, Header Checksum
		26-29, Source IP
		30-33, Destination IP
	UDP header:
		34-35, Source Port
		36-37, Destination Port
		38-39, Length
		40-41, Checksum (Can be set to 0x0000)
