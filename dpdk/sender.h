#ifndef _SENDER_H
#define _SENDER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <error.h>
#include <getopt.h>
#include <unistd.h>

#define MAXBUF 2048
#define SLEEPUS 20

struct pseudo_hdr {
	uint32_t source;
	uint32_t dst;
	uint8_t placeholder;
	uint8_t protocol;
	uint16_t udp_length;
};

const char usage_str[] = "sender usage:\n"
                        "  --help -h for more information.\n"
                        "  --verbose -v with verbose output\n"
                        "  --srcIP -s source IP address\n"
                        "  --dstIP -d destination IP address\n"
                        "  --srcPort -a  source UDP port\n"
                        "  --dstPort -b destination UDP port\n"
                        "  --srcMac -e source MAC address\n"
                        "  --dstMac -f destination MAC address\n"
			"  --msgsize -m message size\n"
			"  --ethname -n src ethernet name\n";
void usage();
void set_default_srcmac(struct ether_header *eth);
void set_default_dstmac(struct ether_header *eth);
void set_default_srcip(struct iphdr *iph);
void set_default_dstip(struct iphdr *iph);
void set_default_srcudp(struct udphdr *udph);
void set_default_dstudp(struct udphdr *udph);
unsigned short csum(unsigned short *buf, int nwords);
void set_default_msgsize(int *size);
#endif
