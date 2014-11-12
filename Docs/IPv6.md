
IPv6
====

IPv6 (Internet Protocol version 6) is the successor of IPv4. Yep, the IETF (Internet Engineering Task Force) has been skipping version numbers since before Microsoft thought it was cool.

Anatomy of an IPv6 packet
--------

IPv6 acts as a building block for more complex protocols.

IPv6 packet layout:

	Ethernet header:
		0-5,   Destination MAC
		6-11,  Source MAC
		12-13, Type IPv6 (0x86DD)
	IPv6 header:
		14-17, Version/Class/Flow Label
			Bits 0-3		Version, set to 6 (0110)
			Bits 4-11		Traffic Class
			Bits 12-31		Flow Label
		18-19, Payload Length
		20,    Next Header
		21,    Hop Limit
		22-37, Source Address
		38-53, Destination Address

