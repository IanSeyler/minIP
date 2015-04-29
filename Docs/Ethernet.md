
Ethernet
========

Ethernet (refered to those in the industry as "IEEE 802.3") is the physical connection to your local network.

Ethernet header layout:

	Ethernet header:
		0-5,   Destination MAC
		6-11,  Source MAC
		12-13, EtherType/Length
	Ethernet data:
		14-1500

Ethernet header diagram:
                                                         
	  ┌───────────────────────────────────────────────────────────────┐
	  │0                   1                   2                   3  │
	  │0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1│
	  ├───────────────────────────────────────────────────────────────┤
	  │                    Destination MAC Address                    │
	  ├───────────────────────────────┬───────────────────────────────┤
	  │   Destination MAC Continued   │      Source MAC Address       │
	  ├───────────────────────────────┴───────────────────────────────┤
	  │                     Source MAC Continued                      │
	  ├───────────────────────────────┬───────────────────────────────┘
	  │           EtherType           │                                
	  └───────────────────────────────┘                                