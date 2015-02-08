nclude <stdio.h>
#include <pcap.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*Example code taken from tcpdump.com and modified */

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* ethernet headers are always exactly 14 bytes */
#define SIZE_ETHERNET 14

#define IPv4_ETHERTYPE 0x800

#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)		(((ip)->ip_vhl) >> 4)

/* Ethernet header */
struct sniff_ethernet {
	u_char          ether_dhost[ETHER_ADDR_LEN];	/* Destination host address */
	u_char          ether_shost[ETHER_ADDR_LEN];	/* Source host address */
	u_short         ether_type;	/* IP? ARP? RARP? etc */
};

	/* IP header */
struct sniff_ip {
	u_char          ip_vhl;	/* version << 4 | header length >> 2 */
	u_char          ip_tos;	/* type of service */
	u_short         ip_len;	/* total length */
	u_short         ip_id;	/* identification */
	u_short         ip_off;	/* fragment offset field */
#define IP_RF 0x8000		/* reserved fragment flag */
#define IP_DF 0x4000		/* dont fragment flag */
#define IP_MF 0x2000		/* more fragments flag */
#define IP_OFFMASK 0x1fff	/* mask for fragmenting bits */
	u_char          ip_ttl;	/* time to live */
	u_char          ip_p;	/* protocol */
	u_short         ip_sum;	/* checksum */
	struct in_addr  ip_src, ip_dst;	/* source and dest address */
};

int
main(int argc, char *argv[])
{
	char           *filename, errbuf[PCAP_ERRBUF_SIZE];
	pcap_t         *handle;
	const u_char   *packet;	/* The actual packet */
	struct pcap_pkthdr header;	/* The header that pcap gives us */
	const struct sniff_ethernet *ethernet;	/* The ethernet header */
	const struct sniff_ip *ip;	/* The IP header */
	u_int           size_ip;

	if (argc < 2) {
		fprintf(stderr, "Usage: readfile filename\n");
		return (2);
	}
	filename = argv[1];
	handle = pcap_open_offline(filename, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open file %s: %s\n", filename, errbuf);
		return (2);
	}
	while (1) {
/* Grab a packet */
		packet = pcap_next(handle, &header);
		if (packet == NULL) {	/* End of file */
			break;
		}
		ethernet = (struct sniff_ethernet *) (packet);
		if (ntohs(ethernet->ether_type) == IPv4_ETHERTYPE) {
			ip = (struct sniff_ip *) (packet + SIZE_ETHERNET);
			size_ip = IP_HL(ip) * 4;
			if (IP_V(ip) == 4) {	/* Yes, that's an IP packet */
				/* Print its length */
				printf
				    ("Got an IPv4 packet with length of [%d] and transport protocol [%d]\n",
				     header.len, ip->ip_p);
			}
		}
	}
	/* And close the session */
	pcap_close(handle);

	return (0);
}
