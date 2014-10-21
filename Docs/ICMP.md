
ICMP
========

ICMP (Internet Control Message Protocol)


Anatomy of an ICMP request/reply
--------

ICMP builds upon IP so you'll see familiar IP headers here.

ICMP ping request packet layout:

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
		23,    Protocol (0x01 for ICMP)
		24-25, Header Checksum
		26-29, Source IP
		30-33, Destination IP
	ICMP header:
		34,    Type (0x08, Ping request)
		35,    Code
		36-37, Checksum
		38-39, Identifier
		40-41, Sequence number
		42-49, Timestamp
		50-??, Data
		
ICMP ping reply packet layout:

	Ethernet header:
		0-5,   Destination MAC
		6-11,  Source MAC
		12-13, Type IP (0x0800)
	IP header:
		14,    Version/Header Length
		15,    Differentiated Services Field
		16-17, Total Length
		18-19, Identification
		20-21, Flags/Fragment Offset
		22,    Time To Live
		23,    Protocol (0x01 for ICMP)
		24-25, Header Checksum
		26-29, Source IP
		30-33, Destination IP
	ICMP header:
		34,    Type (0x00, Ping reply)
		35,    Code
		36-37, Checksum
		38-39, Identifier
		40-41, Sequence number
		42-49, Timestamp
		50-??, Data
