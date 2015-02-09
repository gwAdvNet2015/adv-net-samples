#include <ctype.h>
#include<netinet/in.h>
#include<arpa/inet.h>


#include <sys/types.h>
/*Ethernet Address Lenghth: 6 bytes */
#define ETHER_ADDR_LEN 6

/*Ethernet header size: 14 bytes*/
#define SIZE_ETHERNET 14

/*Default snap length: Max bytes to capture per packet*/
#define SNAP_LEN 1518



/*Ethernet Header */
struct sniff_ethernet {

    /*Destination host address*/
    u_char ether_dhost[ETHER_ADDR_LEN];
    
    /*Source host address*/
    u_char ether_shost[ETHER_ADDR_LEN];

    /*IP? ARP? RARP? etc*/
    u_short ether_type; 

};


/*IP Header */
struct sniff_ip{

    /*Version << 4 | header length >>2*/
    u_char ip_vhl;

    /*Type of service*/
    u_char ip_tos;

    /*total length*/
    u_short ip_len; 

    /*identification*/
    u_short ip_id;

    /*fragment offset field*/
    u_short ip_off;

    /*reserved fragment flag*/
    #define IP_RF 0x8000 

    /*dont fragment flag*/
    #define IP_DF 0x4000

    /*more fragment flag*/
    #define IP_MF 0x2000

    /*mask for fragmenting bits*/
    #define IP_OFFMASK 0x1fff

    /*time to live*/
    u_char ip_ttl;

    /* protocol */
    u_char ip_p;

    /*checksum*/
    u_short ip_sum; 

    /*Source and destination address, struct definition is in inet.h\in.h */
    struct in_addr ip_src;
    struct in_addr ip_dst;
};



#define IP_HL(ip)        (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)         (((ip)->ip_vhl) >> 4)


/*TCP header*/
typedef u_int tcp_seq;

struct sniff_tcp{

    /*source port*/
    u_short th_sport;

    /*destination port*/
    u_short th_dport;

    /*sequence port*/
    tcp_seq th_seq;

    /*acknowledge number */
    tcp_seq th_ack;

    /*data offset, rsvd */
    u_char th_offx2;

    #define TH_OFF(th) (((th)->th_offx2 & 0xf0) >>4)
    
    u_char th_flags;

    #define TH_FIN 0x01
    #define TH_SYN 0x02
    #define TH_RST 0x04
    #define TH_PUSH 0x08
    #define TH_ACK 0x10
    #define TH_URG 0x20
    #define TH_ECE 0x40
    #define TH_CWR 0x80
    #define TH_FLAGS             (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    
    /*window*/
    u_short th_win;

    /*checksum*/
    u_short th_sum;
    
    /*urgent pointer*/
    u_short th_urp;

};



/*Function prototype for callback function*/
void got_packet(u_char *args, const struct pcap_pkthdr * header, const u_char *packet);


