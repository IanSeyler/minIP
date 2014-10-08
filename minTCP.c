/* MinTCP */
/* Written by Ian Seyler */

// Linux compile : gcc minTCP.c -o minTCP
// Linux usage: ./minTCP eth1 192.168.0.99 255.255.255.0

/*
Ethernet packet structure

 MAC Header
 6 bytes - Destination MAC Address
 6 bytes - Source MAC Address
 2 bytes - EtherType

 Data
 46-1498 bytes - Data
*/

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
unsigned char* buffer;
int s; // Socket variable
int running = 1, c;
struct sockaddr_ll sa;
struct ifreq ifr;

char userinput[160], command[20];

int main(int argc, char *argv[])
{
	printf("MinTCP v0.1 (2014 10 07)\n");
	printf("Written by Ian Seyler @ Return Infinity\n\n");

	/* first argument needs to be a NIC */
	if (argc < 2)
	{
		printf("Please specify an Ethernet device\n");
		exit(0);
	}

        net_init(argv[1]); // Get us a socket that can handle raw Ethernet frames

	printf("This server is: %02X:%02X:%02X:%02X:%02X:%02X\n\n", src_MAC[0], src_MAC[1], src_MAC[2], src_MAC[3], src_MAC[4], src_MAC[5]);

	while(running == 1)
	{
		printf("> ");		// Print the prompt

		memset(command, 0, 20);
		fgets(userinput, 100, stdin);	// Get up to 100 chars from the keyboard
		sscanf(userinput, "%s", command);	// Grab the first word in the string

	}

	printf("\n");
	close(s);
	return 0;
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

	buffer = (void*)malloc(ETH_FRAME_LEN);

        /* We should now have a working port to send/recv raw frames */
	return 0;
}


/* net_send - Send a raw Ethernet packet */
int net_send(unsigned char* data, unsigned int bytes)
{
	// We should make sure the smallest packet sent is 64 bytes in total
	return (sendto(s, data, bytes, 0, (struct sockaddr *)&sa, sizeof (sa)));
}


/* net_recv - Receive a raw Ethernet packet */
int net_recv(unsigned char* data)
{
	int retval;
	retval = recvfrom(s, data, ETH_FRAME_LEN, 0, 0, 0);
//	printf("\nI saw %d bytes!\n", c);
//	for (tint=0; tint<c; tint++)
//	{
//		printf("%02X", buffer[tint]);
//	}
	return retval;
}
