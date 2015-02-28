/* minIP */
/* Written by Ian Seyler */

// Linux compile : gcc minIP.c -o minIP
// Linux usage: ./minIP eth1 192.168.0.99 255.255.255.0

/* Global Includes */
#include <stdio.h>
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

/* Global functions */
unsigned short checksum(unsigned char* data, unsigned int bytes);
int net_init(char *interface);
int net_send(unsigned char* data, unsigned int bytes);
int net_recv(unsigned char* data);

/* Global defines */
#undef ETH_FRAME_LEN
#define ETH_FRAME_LEN 1518

/* Global variables */
unsigned char src_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // server address
unsigned char dst_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // node address
unsigned char dst_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char src_IP[4] = {0, 0, 0, 0};
unsigned char src_SN[4] = {0, 0, 0, 0};
unsigned char src_GW[4] = {0, 0, 0, 0};
unsigned char dst_IP[4] = {0, 0, 0, 0};
unsigned char* buffer;
unsigned char* tosend;
unsigned short tshort, checksumval;
int s; // Socket variable
int running = 1, c, retval;
struct sockaddr_ll sa;
struct ifreq ifr;

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
	printf("minIP v0.1 (2014 10 08)\n");
	printf("Written by Ian Seyler @ Return Infinity\n\n");

	/* first argument needs to be a NIC */
	if (argc < 4)
	{
		printf("Insufficient arguments!\n");
		printf("%s interface ip subnet\n", argv[0]);
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
		if (retval > 0)
		{
			if (ntohs(*(unsigned short *) &buffer[12]) == 0x0806) // ARP
			{
			//	printf("\nARP - ");
				if (buffer[21] == 0x01)
				{
			//		printf("Request - Who is %d.%d.%d.%d? Tell %d.%d.%d.%d", buffer[38], buffer[39], buffer[40], buffer[41], buffer[28], buffer[29], buffer[30], buffer[31]);
					if ((buffer[38] == src_IP[0]) & (buffer[39] == src_IP[1]) & (buffer[40] == src_IP[2]) & (buffer[41] == src_IP[3]))
					{
			//			printf(" - Looking for me?");
						memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
						memcpy((void*)tosend, (void*)buffer+6, 6); // copy the incoming MAC to destination
						memcpy((void*)tosend+6, (void*)src_MAC, 6); // copy the source MAC
						memcpy((void*)tosend+32, (void*)buffer+22, 10); // copy sender+mac ID to target
						tosend[21] = 0x02; // ARP reply
						memcpy((void*)tosend+22, (void*)src_MAC, 6); // copy the source MAC
						memcpy((void*)tosend+28, (void*)buffer+38, 4); // copy IP address
						net_send(tosend, 42);
					}
				}
				else if (buffer[21] == 0x02)
				{
			//		printf("Response");
				}
			}
			else if (ntohs(*(unsigned short *) &buffer[12]) == 0x0800) // buffer[12] == 0x08 & buffer[13] == 0x00
			{
				printf("\nIPv4 - ");
				if(buffer[23] == 0x01)
				{
					printf("ICMP - ");
					if(buffer[34] == 0x08)
					{
						printf("Request");
						if ((buffer[30] == src_IP[0]) & (buffer[31] == src_IP[1]) & (buffer[32] == src_IP[2]) & (buffer[33] == src_IP[3]))
						{
							// Reply to the ping request
							printf(" - Replying!");
							memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
							memcpy((void*)tosend, (void*)buffer+6, 6); // copy the incoming MAC to destination
							memcpy((void*)tosend+6, (void*)buffer, 6); // copy the source MAC
							memcpy((void*)tosend+30, (void*)buffer+26, 4); // copy the incoming IP to destination
							memcpy((void*)tosend+26, (void*)buffer+30, 4); // copy the destination IP to source
							// No IP header checksum calc since the header contents were only shifted around
							tosend[34] = 0x00; // ICMP type = reply
							tosend[36] = 0x00; // clear ICMP checksum
							tosend[37] = 0x00; // clear ICMP checksum
							checksumval = checksum(&tosend[34], retval-14-20); // Frame length - MAC header - IP header
							memcpy((void*)tosend+36, (void*)&checksumval, 2);
							net_send(tosend, retval); // send the response
						}
					}
					else if (buffer[34] == 0x00)
					{
						printf("Reply");
					}
					else
					{
						printf("Other");
					}
				}
				else if(buffer[23] == 0x06)
				{
					printf("TCP");
					if ((buffer[47] & 0x02) == 0x02)
					{
						printf(" - SYN");
						memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
						memcpy((void*)tosend, (void*)buffer+6, 6); // copy the incoming MAC to destination
						memcpy((void*)tosend+6, (void*)buffer, 6); // copy the source MAC
						memcpy((void*)tosend+30, (void*)buffer+26, 4); // copy the incoming IP to destination
						memcpy((void*)tosend+26, (void*)buffer+30, 4); // copy the destination IP to source
						memcpy((void*)&tshort, (void*)buffer+16, 2); // extract the packet length
						memcpy((void*)tosend+34, (void*)buffer+36, 2); // copy destination port to source
						memcpy((void*)tosend+36, (void*)buffer+34, 2); // copy source port to destination
						memcpy((void*)&tint, (void*)buffer+38, 4); // extract sequence number
						tint = ntohl(tint);
						tint++; // increment sequence number
						tint = htonl(tint);
						memcpy((void*)tosend+42, (void*)&tint, 4); // store updated sequence number as acknowledgement number
						tosend[47] |= 0x10; // set TCP ACK
						tosend[50] = 0x00; // clear TCP checksum
						tosend[51] = 0x00; // clear TCP checksum
						tosend[retval] = 0x00;
						tosend[retval+1] = 16;
						tosend[retval+2] = 0;
						tosend[retval+3] = 34;
						checksumval = checksum(&tosend[26], retval-26+4); // Start checksum at Source IP so we only need to build the tail of the pseudo header
						memcpy((void*)tosend+50, (void*)&checksumval, 2);
						net_send(tosend, retval);
					}
					else if ((buffer[47] & 0x10) == 0x10)
					{
						printf(" - ACK");
					}
				}
				else if (buffer[23] == 0x11)
				{
					printf("UDP");
				}
				else
				{
					printf("Other");
				}
			}
			else if (ntohs(*(unsigned short *) &buffer[12]) == 0x86DD) // buffer[12] == 0x86 & buffer[13] == 0xDD
			{
				printf("\nIPv6");
			}
		}
	}

	printf("\n");
	close(s);
	return 0;
}


unsigned short checksum(unsigned char* data, unsigned int bytes)
{
	unsigned int i, sum = 0;

	for (i=0; i<bytes-1; i+=2) // Add up the words
		sum += *(unsigned short *) &data[i];

	if (bytes & 1) // Add the left-over byte if there is one
		sum += (unsigned char) data[i];
	
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
