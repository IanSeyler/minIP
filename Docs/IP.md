
IP
========

IP (Internet Protocol) is, well, how the internet communicates. This is also known as IPv4 or "Internet Classic".

Anatomy of an IP packet
--------

IP acts as a building block for more complex protocols.

IP packet layout:

	Ethernet header:
		0-5,   Destination MAC
		6-11,  Source MAC
		12-13, Type IP (0x0800)
	IP header:
		14,    Version/Header Length
			Bits 0-3	Version (Set to 0x4)
			Bits 4-7	Header length in 32-bit words (Minimum 5 aka 20 bytes)
		15,    Differentiated Services Field (0x00)
		16-17, Total Length
		18-19, Identification
		20-21, Flags/Fragment Offset
			Bits 0-2	Flags
			Bits 3-15	Fragment Offset
		22,    Time To Live (Usually 0x40)
		23,    Protocol
		24-25, Header Checksum
		26-29, Source IP
		30-33, Destination IP

