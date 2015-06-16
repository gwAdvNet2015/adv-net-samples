#include "sender.h"

static struct option long_options[] = {
	{"srcIP", required_argument, 0, 's'},
	{"dstIP", required_argument, 0, 'd'},
	{"srcPort", required_argument, 0, 'a'},
	{"dstPort", required_argument, 0, 'b'},
	{"dstMac", required_argument, 0, 'f'},
	{"msgSize", required_argument, 0, 'm' },
	{"ethname", required_argument, 0, 'n'},
	{"verbose", no_argument, 0, 'v'}, 
	{"help", no_argument, 0, 'h'},
	{0,		0, 		0}
};

char sendbuf[MAXBUF];

void usage() {
	fprintf(stderr, usage_str);
}

void set_default_srcmac(struct ether_header *eth) {
	eth->ether_shost[0] = 0x90; 
	eth->ether_shost[1] = 0xe2;
	eth->ether_shost[2] = 0xba;
	eth->ether_shost[3] = 0x4a;
	eth->ether_shost[4] = 0xe6;
	eth->ether_shost[5] = 0x2f;
}
	
void set_default_dstmac(struct ether_header *eth) {
	eth->ether_dhost[0] = 0xec;
	eth->ether_dhost[1] = 0xf4;
	eth->ether_dhost[2] = 0xbb;
	eth->ether_dhost[3] = 0xc8;
	eth->ether_dhost[4] = 0x99;
	eth->ether_dhost[5] = 0x88;
}

void set_default_srcip(struct iphdr *iph) {
	iph->saddr = inet_addr("10.1.1.25");
}

void set_default_dstip(struct iphdr *iph) {
	iph->daddr = inet_addr("10.1.1.27");
}

void set_default_srcudp(struct udphdr *udph) {
	udph->source = htons(atoi("1234"));	
}
void set_default_dstudp(struct udphdr *udph) {
	udph->dest = htons(atoi("5678"));	
}
 
void set_default_msgsize(int *size) {
	*size = 64;
}

void set_default_ethname(char *ethname) {
	strncpy(ethname, "p2p2", IFNAMSIZ-1);
}

unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main(int argc, char **argv) {
	int opt = 0;
	int verbose = 0;
	struct ether_header *eth;
	struct iphdr *iph;
	struct udphdr *udph;
	int sockfd;
	struct sockaddr_ll socket_address;
	int msgsize;
	struct ifreq if_idx;
	struct pseudo_hdr psh;
	char  *pseudogram;

	memset(&if_idx, 0, sizeof(struct ifreq));

	memset(sendbuf, 0, sizeof(sendbuf));
	eth = (struct ether_header *)sendbuf;
	iph = (struct iphdr *)(sendbuf + sizeof(struct ether_header));
	udph = (struct udphdr *)(sendbuf + sizeof(struct iphdr) + sizeof(struct ether_header));

	set_default_srcmac(eth);
	set_default_dstmac(eth);
	set_default_srcip(iph);
	set_default_dstip(iph);
	set_default_srcudp(udph);
	set_default_dstudp(udph);
	set_default_msgsize(&msgsize);
	set_default_ethname(if_idx.ifr_name);

	while((opt = getopt_long(argc, argv, "s:d:a:b:f:m:n:hv", long_options, NULL)) != -1) {
		switch (opt) {
			case 'f':
				sscanf(optarg, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", 
					&eth->ether_dhost[0], &eth->ether_dhost[1],
					&eth->ether_dhost[2], &eth->ether_dhost[3],
					&eth->ether_dhost[4], &eth->ether_dhost[5]);
				break;

			case 's':
				iph->saddr = inet_addr(optarg);
				break;

			case 'd':
				iph->daddr = inet_addr(optarg);
				break;

			case 'a':
				udph->source = htons(atoi(optarg));
				break;

			case 'b':
				udph->dest = htons(atoi(optarg));
				break;

			case 'm':
				msgsize = atoi(optarg);
				break;

			case 'n':
				strncpy(if_idx.ifr_name, optarg, IFNAMSIZ-1);	

			case 'v':
				verbose = 1;
				break;
			
			case 'h':
			default:
				usage();
				break;
		}
	}

	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("socket");
		exit(1);
	}

	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
    		perror("SIOCGIFINDEX");
		exit(1);
	}

	eth->ether_type = htons(ETH_P_IP);
	
	//IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 16; //Low delay
	iph->id = htons(54321);
	iph->ttl = 10;
	iph->protocol = IPPROTO_UDP; //UDP
	iph->check = 0;
	
	//UDP Header
	udph->len = htons(msgsize - sizeof(struct ether_header) - sizeof(struct iphdr));
	//Now the UDP checksum using the pseudo header
	psh.source = iph->saddr;
	psh.dst = iph->daddr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_UDP;
	psh.udp_length = udph->len;
	     
	int psize = sizeof(struct pseudo_hdr) + msgsize - 
			sizeof(struct ether_header) - sizeof(struct iphdr);
	pseudogram = malloc(psize);
	if (pseudogram == NULL){
		perror("malloc");
		exit(1);
	}
	     
	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_hdr));
	memcpy(pseudogram + sizeof(struct pseudo_hdr) , udph , msgsize - 
		sizeof(struct ether_header) - sizeof(struct iphdr));
	     
	//udph->check = csum( (unsigned short*) pseudogram , psize/2);
	udph->check = csum( (unsigned short*) pseudogram , psize/2);
		
	iph->tot_len = htons(msgsize - sizeof(struct ether_header));
	iph->check = csum((unsigned short *)(sendbuf+sizeof(struct ether_header)), 
			sizeof(struct iphdr)/2);

	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;
	socket_address.sll_addr[0] = eth->ether_dhost[0];
	socket_address.sll_addr[1] = eth->ether_dhost[1];
	socket_address.sll_addr[2] = eth->ether_dhost[2];
	socket_address.sll_addr[3] = eth->ether_dhost[3];
	socket_address.sll_addr[4] = eth->ether_dhost[4];
	socket_address.sll_addr[5] = eth->ether_dhost[5];

	if (verbose) {
		printf("dst mac = %02x:%02x:%02x:%02x:%02x:%02x\n", socket_address.sll_addr[0], 
		        socket_address.sll_addr[1], socket_address.sll_addr[2], 
			socket_address.sll_addr[3], socket_address.sll_addr[4], 
			socket_address.sll_addr[5]);
		printf("src mac = %02x:%02x:%02x:%02x:%02x:%02x\n", eth->ether_shost[0], 
			eth->ether_shost[1], eth->ether_shost[2], eth->ether_shost[3],
			eth->ether_shost[4], eth->ether_shost[5]);
	}
	while (1) {
		if (sendto(sockfd, sendbuf, msgsize, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
			perror("send");
			exit(1);
		}
		printf("sending out\n");
		usleep(SLEEPUS);
	}

	free(pseudogram);
}
