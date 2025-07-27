
# DHCP

DHCP (Dynamic Host Configuration Protocol) also known as [RFC 2131](http://tools.ietf.org/html/rfc2131).


## The DHCP Sequence

1) DHCPDiscover - Request from Client
2) DHCPOffer - Offer from Server
3) DHCPRequest - Selection from Client
4) DHCPAck - Acknowledgement from Server


## Anatomy of the DHCP packets

DHCP builds upon UDP so you'll see familiar UDP and IP headers here.


### DHCPDiscover Packet

```
Ethernet header:
 0-5,   Destination MAC (0xFFFFFFFFFFFF)
 6-11,  Source MAC (This MAC)
 12-13, EtherType (IPv4 0x0800)
IP header:
 14,    Version/Header Length (Usually 0x45)
 15,    Differentiated Services Field (0x00)
 16-17, Total Length
 18-19, Identification
 20-21, Flags/Fragment Offset
 22,    Time To Live (Usually 0x40)
 23,    Protocol (0x11 for UDP)
 24-25, Header Checksum
 26-29, Source IP (0.0.0.0)
 30-33, Destination IP (255.255.255.255)
UDP header:
 34-35, Source Port (68)
 36-37, Destination Port (67)
 38-39, Length
 40-41, Checksum (Can be set to 0x0000)
Data:
 42,      OP (0x01 - Boot Request)
 43,      HTYPE (0x01 - Ethernet)
 44,      HLEN (0x06)
 45,      HOPS (0x00)
 46-49,   XID (0x35BA1681) Transaction ID - A random number from the client
 50-51,   SECS (0x0000) Seconds Elapsed
 52-53,   FLAGS (0x0000)
 54-57,   CIADDR (0x00000000) Client IP Address
 58-61,   YIADDR (0x00000000)
 62-65,   SIADDR (0x00000000) Server IP Address
 66-69,   GIADDR (0x00000000)
 70-85,   CHADDR (This MAC)
 86-277,  0
 278-281, Magic Cookie (0x63825363)
```


### DHCPOffer Packet

```
Ethernet header:
 0-5,   Destination MAC (This MAC)
 6-11,  Source MAC (DHCP Server MAC)
 12-13, EtherType (IPv4 0x0800)
IP header:
 14,    Version/Header Length (Usually 0x45)
 15,    Differentiated Services Field (0x00)
 16-17, Total Length
 18-19, Identification
 20-21, Flags/Fragment Offset
 22,    Time To Live (Usually 0x40)
 23,    Protocol (0x11 for UDP)
 24-25, Header Checksum
 26-29, Source IP (0.0.0.0)
 30-33, Destination IP (255.255.255.255)
UDP header:
 34-35, Source Port (67)
 36-37, Destination Port (68)
 38-39, Length
 40-41, Checksum (Can be set to 0x0000)
Data:
 42,      OP (0x02 - Boot Reply)
 43,      HTYPE (0x01 - Ethernet)
 44,      HLEN (0x06)
 45,      HOPS (0x00)
 46-49,   XID (0x35BA1681)
 50-51,   SECS (0x0000)
 52-53,   FLAGS (0x0000)
 54-57,   CIADDR (0x00000000)
 58-61,   YIADDR (Your new IP)
 62-65,   SIADDR (DHCP Server IP)
 66-69,   GIADDR (0x00000000)
 70-85,   CHADDR (This MAC)
 86-277,  0
 278-281, Magic Cookie (0x63825363)
```


### DHCPRequest Packet

```
Ethernet header:
 0-5,   Destination MAC (0xFFFFFFFFFFFF)
 6-11,  Source MAC (This MAC)
 12-13, EtherType (IPv4 0x0800)
IP header:
 14,    Version/Header Length (Usually 0x45)
 15,    Differentiated Services Field (0x00)
 16-17, Total Length
 18-19, Identification
 20-21, Flags/Fragment Offset
 22,    Time To Live (Usually 0x40)
 23,    Protocol (0x11 for UDP)
 24-25, Header Checksum
 26-29, Source IP (0.0.0.0)
 30-33, Destination IP (255.255.255.255)
UDP header:
 34-35, Source Port (68)
 36-37, Destination Port (67)
 38-39, Length
 40-41, Checksum (Can be set to 0x0000)
Data:
 42,      OP (0x01 - Boot request)
 43,      HTYPE (0x01)
 44,      HLEN (0x06)
 45,      HOPS (0x00)
 46-49,   XID (0x35BA1681)
 50-51,   SECS (0x0000)
 52-53,   FLAGS (0x0000)
 54-57,   CIADDR (0x00000000)
 58-61,   YIADDR (0x00000000)
 62-65,   SIADDR (DHCP Server IP)
 66-69,   GIADDR (0x00000000)
 70-85,   CHADDR (This MAC)
 86-277,  0
 278-281, Magic Cookie (0x63825363)
```


### DHCPAck Packet

```
Ethernet header:
 0-5,   Destination MAC (This MAC)
 6-11,  Source MAC (DHCP Server MAC)
 12-13, EtherType (IPv4 0x0800)
IP header:
 14,    Version/Header Length (Usually 0x45)
 15,    Differentiated Services Field (0x00)
 16-17, Total Length
 18-19, Identification
 20-21, Flags/Fragment Offset
 22,    Time To Live (Usually 0x40)
 23,    Protocol (0x11 for UDP)
 24-25, Header Checksum
 26-29, Source IP (DHCP Server IP)
 30-33, Destination IP (Client IP)
UDP header:
 34-35, Source Port (67)
 36-37, Destination Port (68)
 38-39, Length
 40-41, Checksum (Can be set to 0x0000)
Data:
 42,      OP (0x02)
 43,      HTYPE (0x01)
 44,      HLEN (0x06)
 45,      HOPS (0x00)
 46-49,   XID (0x35BA1681)
 50-51,   SECS (0x0000)
 52-53,   FLAGS (0x0000)
 54-57,   CIADDR (0x00000000)
 58-61,   YIADDR (Your new IP)
 62-65,   SIADDR (DHCP Server IP)
 66-69,   GIADDR (0x00000000)
 70-85,   CHADDR (This MAC)
 86-277,  0
 278-281, Magic Cookie (0x63825363)
```

