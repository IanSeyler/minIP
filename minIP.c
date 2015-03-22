/* minIP */
/* Written by Ian Seyler */

// Linux compile : gcc minIP.c -o minIP
// Linux usage: ./minIP eth1 192.168.0.99 255.255.255.0 192.168.0.1

/* Global Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <fcntl.h>
#include <errno.h>

/* Typedefs */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Global functions */
u16 checksum(u8* data, u16 bytes);
int net_init(char *interface);
int net_send(unsigned char* data, unsigned int bytes);
int net_recv(unsigned char* data);
u16 swap16(u16 in);
u32 swap32(u32 in);

/* Global defines */
#undef ETH_FRAME_LEN
#define ETH_FRAME_LEN 1518
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPv4 0x0800
#define ETHERTYPE_IPv6 0x86DD
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define PROTOCOL_IP_ICMP 1
#define PROTOCOL_IP_TCP 6
#define PROTOCOL_IP_UDP 11
#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8

#define TCP_ACK 16
#define TCP_RST 4
#define TCP_SYN 2
#define TCP_FIN 1

/* Global variables */
u8 src_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 dst_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 dst_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8 src_IP[4] = {0, 0, 0, 0};
u8 src_SN[4] = {0, 0, 0, 0};
u8 src_GW[4] = {0, 0, 0, 0};
u8 dst_IP[4] = {0, 0, 0, 0};
unsigned char* buffer;
unsigned char* tosend;
unsigned short tshort, checksumval;
int s; // Socket variable
int running = 1, c, retval;
struct sockaddr_ll sa;
struct ifreq ifr;

/* Global structs */
#pragma pack(1)
typedef struct eth_header {
	u8 dest_mac[6];
	u8 src_mac[6];
	u16 type;
} eth_header; // 14 bytes
typedef struct arp_packet {
	eth_header ethernet;
	u16 hardware_type;
	u16 protocol;
	u8 hardware_size;
	u8 protocol_size;
	u16 opcode;
	u8 sender_mac[6];
	u8 sender_ip[4];
	u8 target_mac[6];
	u8 target_ip[4];
} arp_packet; // 28 bytes
typedef struct ipv4_packet {
	eth_header ethernet;
	u8 version;
	u8 dsf;
	u16 total_length;
	u16 id;
	u16 flags;
	u8 ttl;
	u8 protocol;
	u16 checksum;
	u8 src_ip[4];
	u8 dest_ip[4];
} ipv4_packet; // 20 bytes since we don't support options
typedef struct icmp_packet {
	ipv4_packet ipv4;
	u8 type;
	u8 code;
	u16 checksum;
	u16 id;
	u16 sequence;
	u64 timestamp;
	u8 data;
} icmp_packet;
typedef struct udp_packet {
	ipv4_packet ipv4;
	u16 src_port;
	u16 dest_port;
	u16 length;
	u16 checksum;
	u8 data;
} udp_packet;
typedef struct tcp_packet {
	ipv4_packet ipv4;
	u16 src_port;
	u16 dest_port;
	u32 seqnum;
	u32 acknum;
	u8 data_offset;
	u8 flags;
	u16 window;
	u16 checksum;
	u16 urg_pointer;
	u8 data;
} tcp_packet;


/* Default HTTP page with HTTP headers */
char webpage[] =
"HTTP/1.0 200 OK\n"
"Server: BareMetal (http://www.returninfinity.com)\n"
"Content-type: text/html\n"
"\n"
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<title>Hello world</title>\n"
"</head>\n"
"<body>\n"
"Hello World!\n"
"</body>\n"
"</html>\n";

unsigned int tint, tint0, tint1, tint2, tint3;

int main(int argc, char *argv[])
{
	printf("minIP v0.1 (2015 03 19)\n");
	printf("Written by Ian Seyler @ Return Infinity\n\n");

	/* first argument needs to be a NIC */
	if (argc < 5)
	{
		printf("Insufficient arguments!\n");
		printf("%s interface ip subnet gw\n", argv[0]);
		exit(0);
	}

	/* Parse the IP and Subnet */
	sscanf(argv[2], "%u.%u.%u.%u", &tint0, &tint1, &tint2, &tint3);
	src_IP[0] = tint0;
	src_IP[1] = tint1;
	src_IP[2] = tint2;
	src_IP[3] = tint3;
	sscanf(argv[3], "%u.%u.%u.%u", &tint0, &tint1, &tint2, &tint3);
	src_SN[0] = tint0;
	src_SN[1] = tint1;
	src_SN[2] = tint2;
	src_SN[3] = tint3;
	sscanf(argv[4], "%u.%u.%u.%u", &tint0, &tint1, &tint2, &tint3);
	src_GW[0] = tint0;
	src_GW[1] = tint1;
	src_GW[2] = tint2;
	src_GW[3] = tint3;

	net_init(argv[1]); // Get us a socket that can handle raw Ethernet frames

	buffer = (void*)malloc(ETH_FRAME_LEN);
	tosend = (void*)malloc(ETH_FRAME_LEN);

	printf("This host:\n");
	printf("HW: %02X:%02X:%02X:%02X:%02X:%02X\n", src_MAC[0], src_MAC[1], src_MAC[2], src_MAC[3], src_MAC[4], src_MAC[5]);
	printf("IP: %u.%u.%u.%u\n", src_IP[0], src_IP[1], src_IP[2], src_IP[3]);
	printf("SN: %u.%u.%u.%u\n", src_SN[0], src_SN[1], src_SN[2], src_SN[3]);

	while(running == 1)
	{
		retval = net_recv(buffer);
		eth_header* rx = (eth_header*)buffer;

		if (retval > 0) // Make sure we received a packet
		{
			memset(tosend, 0, ETH_FRAME_LEN); // clear the send buffer
			if (swap16(rx->type) == ETHERTYPE_ARP)
			{
				arp_packet* rx_arp = (arp_packet*)buffer;
				if (swap16(rx_arp->opcode) == ARP_REQUEST)
				{
			//		printf("ARP Request - Who is %d.%d.%d.%d? Tell %d.%d.%d.%d", buffer[38], buffer[39], buffer[40], buffer[41], buffer[28], buffer[29], buffer[30], buffer[31]);
					if (*(u32*)rx_arp->target_ip == *(u32*)src_IP)
					{
						arp_packet* tx_arp = (arp_packet*)tosend;
						// Ethernet
						memcpy(tx_arp->ethernet.dest_mac, rx_arp->sender_mac, 6);
						memcpy(tx_arp->ethernet.src_mac, src_MAC, 6);
						tx_arp->ethernet.type = swap16(ETHERTYPE_ARP);
						// ARP
						tx_arp->hardware_type = swap16(1); // Ethernet
						tx_arp->protocol = swap16(ETHERTYPE_IPv4);
						tx_arp->hardware_size = 6;
						tx_arp->protocol_size = 4;
						tx_arp->opcode = swap16(ARP_REPLY);
						memcpy(tx_arp->sender_mac, src_MAC, 6);
						memcpy(tx_arp->sender_ip, rx_arp->target_ip, 4);
						memcpy(tx_arp->target_mac, rx_arp->sender_mac, 6);
						memcpy(tx_arp->target_ip, rx_arp->sender_ip, 4);
						// Send the reply
						net_send(tosend, 42);
					}
				}
				else if (buffer[21] == ARP_REPLY)
				{
			//		printf("Response");
				}
			}
			else if (swap16(rx->type) == ETHERTYPE_IPv4)
			{
			//	printf("\nIPv4 - ");
				ipv4_packet* rx_ipv4 = (ipv4_packet*)buffer;
				if(rx_ipv4->protocol == PROTOCOL_IP_ICMP)
				{
			//		printf("ICMP - ");
					icmp_packet* rx_icmp = (icmp_packet*)buffer;
					if(rx_icmp->type == ICMP_ECHO_REQUEST)
					{
			//			printf("Request");
						if (*(u32*)rx_icmp->ipv4.dest_ip == *(u32*)src_IP)
						{
							// Reply to the ping request
							icmp_packet* tx_icmp = (icmp_packet*)tosend;
							// Ethernet
							memcpy(tx_icmp->ipv4.ethernet.dest_mac, rx_icmp->ipv4.ethernet.src_mac, 6);
							memcpy(tx_icmp->ipv4.ethernet.src_mac, src_MAC, 6);
							tx_icmp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
							// IPv4
							tx_icmp->ipv4.version = rx_icmp->ipv4.version;
							tx_icmp->ipv4.dsf = rx_icmp->ipv4.dsf;
							tx_icmp->ipv4.total_length = rx_icmp->ipv4.total_length;
							tx_icmp->ipv4.id = rx_icmp->ipv4.id;
							tx_icmp->ipv4.flags = rx_icmp->ipv4.flags;
							tx_icmp->ipv4.ttl = rx_icmp->ipv4.ttl;
							tx_icmp->ipv4.protocol = rx_icmp->ipv4.protocol;
							tx_icmp->ipv4.checksum = rx_icmp->ipv4.checksum; // No need to recalc checksum
							memcpy(tx_icmp->ipv4.src_ip, rx_icmp->ipv4.dest_ip, 4);
							memcpy(tx_icmp->ipv4.dest_ip, rx_icmp->ipv4.src_ip, 4);
							// ICMP
							tx_icmp->type = ICMP_ECHO_REPLY;
							tx_icmp->code = rx_icmp->code;
							tx_icmp->checksum = 0;
							tx_icmp->id = rx_icmp->id;
							tx_icmp->sequence = rx_icmp->sequence;
							tx_icmp->timestamp = rx_icmp->timestamp; 
							memcpy (tx_icmp->data, rx_icmp->data, (swap16(rx_icmp->ipv4.total_length)-20-16)); // IP length - IPv4 header - ICMP header
							tx_icmp->checksum = checksum(&tosend[34], retval-14-20); // Frame length - MAC header - IPv4 header
							// Send the reply
							net_send(tosend, retval);
						}
					}
					else if (rx_icmp->type == ICMP_ECHO_REPLY)
					{
			//			printf("Reply");
					}
					else
					{
			//			printf("Other");
					}
				}
				else if(rx_ipv4->protocol == PROTOCOL_IP_TCP)
				{
					printf("TCP");
					tcp_packet* rx_tcp = (tcp_packet*)buffer;
					if (rx_tcp->flags == TCP_SYN)
					{
						printf(" - SYN");
						tcp_packet* tx_tcp = (tcp_packet*)tosend;
						memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
						// Ethernet
						memcpy(tx_tcp->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
						memcpy(tx_tcp->ipv4.ethernet.src_mac, src_MAC, 6);
						tx_tcp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
						// IPv4
						tx_tcp->ipv4.version = rx_tcp->ipv4.version;
						tx_tcp->ipv4.dsf = rx_tcp->ipv4.dsf;
						tx_tcp->ipv4.total_length = rx_tcp->ipv4.total_length;
						tx_tcp->ipv4.id = rx_tcp->ipv4.id;
						tx_tcp->ipv4.flags = rx_tcp->ipv4.flags;
						tx_tcp->ipv4.ttl = rx_tcp->ipv4.ttl;
						tx_tcp->ipv4.protocol = rx_tcp->ipv4.protocol;
						tx_tcp->ipv4.checksum = rx_tcp->ipv4.checksum; // No need to recalc checksum
						memcpy(tx_tcp->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
						memcpy(tx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
						// TCP
						tx_tcp->src_port = rx_tcp->dest_port;
						tx_tcp->dest_port = rx_tcp->src_port;
						tx_tcp->seqnum = rx_tcp->seqnum;
						tx_tcp->acknum = swap32(swap32(rx_tcp->seqnum)+1);
						tx_tcp->data_offset = rx_tcp->data_offset;
						tx_tcp->flags = TCP_SYN|TCP_ACK;
						tx_tcp->window = rx_tcp->window;
						tx_tcp->checksum = 0;
						tx_tcp->urg_pointer = rx_tcp->urg_pointer;
						 // Build the rest of the TCP pseudo-header
						tosend[retval] = 0; // Reserved
						tosend[retval+1] = 6; // Protocol
						tosend[retval+2] = 0; // TCP length (Header + Data)
						tosend[retval+3] = 44;
						tx_tcp->checksum = checksum(&tosend[26], retval-26+4); // Start checksum at Source IP so we only need to build the tail of the pseudo header
						// Send the reply
						net_send(tosend, retval);
					}
					else if (rx_tcp->flags == TCP_ACK)
					{
						printf(" - ACK");
					}
					printf("\n");
				}
				else if (rx_ipv4->protocol == PROTOCOL_IP_UDP)
				{
			//		printf("UDP");
				}
				else
				{
			//		printf("Other");
				}
			}
			else if (swap16(rx->type) == ETHERTYPE_IPv6)
			{
			//	printf("\nIPv6");
			}
		}
	}

	printf("\n");
	close(s);
	return 0;
}


u16 checksum(u8* data, u16 bytes)
{
	u32 i, sum = 0;

	for (i=0; i<bytes-1; i+=2) // Add up the words
		sum += *(u16 *) &data[i];

	if (bytes & 1) // Add the left-over byte if there is one
		sum += (u8) data[i];

	while (sum >> 16) // Fold total to 16-bits
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ~sum; // Return 1's complement
}


int net_init(char *interface)
{
	/* Open a socket in raw mode */
	s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (s == -1)
	{
		printf("Error: Could not open socket! Check your permissions.\n");
		exit(1);
	}

	/* Which interface are we using? */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, interface, IFNAMSIZ);

	/* Does that interface exist? */
	if (ioctl(s, SIOCGIFINDEX, &ifr) == -1)
	{
		printf("Interface '%s' does not exist.\n", interface);
		close(s);
		exit(1);
	}

	/* Is that interface up? */
	ioctl(s, SIOCGIFFLAGS, &ifr);
	if ((ifr.ifr_flags & IFF_UP) == 0)
	{
		printf("Interface '%s' is down.\n", interface);
		close(s);
		exit(1);
	}

	/* Configure the port for non-blocking */
	if (-1 == fcntl(s, F_SETFL, O_NONBLOCK))
	{
		printf("fcntl (NonBlocking) Warning\n");
		close(s);
		exit(1);
	}

	/* Get the MAC address */
	ioctl(s, SIOCGIFHWADDR, &ifr);
	for (c=0; c<6; c++)
	{
		src_MAC[c] = (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[c]; // This works... but WTF
	}

	/* Write in the structure again */
	ioctl(s, SIOCGIFINDEX, &ifr);

	/* Configure the rest of what we need */
	memset(&sa, 0, sizeof (sa));
	sa.sll_family = AF_PACKET;
	sa.sll_ifindex = ifr.ifr_ifindex;
	sa.sll_protocol = htons(ETH_P_ALL);

	int test_var = 1;
	unsigned char *test_endian = (unsigned char*)&test_var;

 	if (test_endian[0] == 0x00)
	{
		printf("Big Endian system detected! This program will fail horribly.");
		return -1;
	}

	/* We should now have a working port to send/recv raw frames */
	return 0;
}


/* net_send - Send a raw Ethernet packet */
// Wrapper for OS send function
// Returns number of bytes sent
int net_send(unsigned char* data, unsigned int bytes)
{
	return (sendto(s, data, bytes, 0, (struct sockaddr *)&sa, sizeof (sa)));
}


/* net_recv - Receive a raw Ethernet packet */
// Wrapper for OS recv function
// Returns number of bytes read
int net_recv(unsigned char* data)
{
	return (recvfrom(s, data, ETH_FRAME_LEN, 0, 0, 0));
}


/* swap16 - Change endianness on a 16-bit value */
// x86-64 uses little-endian while IP uses big-endian
u16 swap16(u16 in)
{
	u16 out = in<<8 | ((in&0xff00)>>8);
	return out;
}


/* swap32 - Change endianness on a 32-bit value */
// x86-64 uses little-endian while IP uses big-endian
u32 swap32(u32 in)
{
	u32 out = in<<24 | ((in&0xff00)<<8) | ((in&0xff0000)>>8) | ((in&0xff000000)>>24);
	return out;
}


/* EOF */
