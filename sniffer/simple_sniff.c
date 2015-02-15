//
//  simple_sniff2.c
//  
//
//  Created by lucas on 2/8/15.
//  Sourced from this tutorial: http://www.tcpdump.org/pcap.html
//

#include <stdio.h>
#include <pcap.h> //This is the important library
#include "simple_sniff.h"
#include <stdlib.h>
#include <unistd.h>

/*
 * Handle packets. This is our callback
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    /*this counts the number of packets*/
    static int count = 1;
    
    /*declares pointers to packet headers */
    const struct sniff_ethernet *ethernet;  /*The ethernet header*/
    const struct sniff_ip *ip;              /*The IP header */
    const struct sniff_tcp *tcp;            /*The TCP header*/
    const char *payload;                    /*Packet payload*/

    int size_ip;
    int size_tcp;
    int size_payload;

    printf("\nPacket number %d:\n ",count);
    count++;

    /*defines ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    /*define /compute ip header offset*/
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if(size_ip<20){
        printf("     * Invalid IP header length: %u bytes\n",size_ip);
        return;
    }

    /*Print source and dest ip addresses*/
    printf("\tFrom: %s\n",inet_ntoa(ip->ip_src));
    printf("\tTo: %s\n",inet_ntoa(ip->ip_dst));
    /*determine the protocol of packet */
    if(ip->ip_p != IPPROTO_TCP)
    {
        printf("Protocol is not IP\n");
        return;
    }
    
    /*determine tcp header */
    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if(size_tcp < 20)
    {
        printf("    *Invalid TCP header length: %u bytes\n",size_tcp);
        return;
    }

    printf("\tSrc port: %d\n",ntohs(tcp->th_sport));
    printf("\tDst port: %d\n",ntohs(tcp->th_dport));
    printf("\tSeq Num: %u\n\tACK Num: %u\n",ntohl(tcp->th_seq),ntohl(tcp->th_ack));
    printf("\tWindow: %d\n",ntohs(tcp->th_win));


}






int main(int argc,char *argv[])
{

    /*This is the device to sniff on and the error string*/
    char *dev, errbuf[PCAP_ERRBUF_SIZE];

    /*This the session handler*/
    pcap_t *handle;

    /*The location of the compiled filter expression*/
    struct bpf_program fp;

    /*The filter expression*/
    //char filter_exp[] = "port 80";
    char* filter_exp;

    /*The netmask of our sniffing device*/
    bpf_u_int32 mask;

    /*This holds the ip address for the sniffing device*/
    bpf_u_int32 net;

    /*This is the header that pcap gives us*/
    struct pcap_pkthdr header;

    /*The actual packet we receive*/
    const u_char *packet;

    /* The number of packets to be captured*/
    int num_packets = 15;

    int filter_flag = 0;
    int o; /*Arguments */
    char *help = "Unknown Argument:\n"
                 "Arguments:\n "
                 "\t-i  interface to listen on Ex. eth0\n"
                 "\t-e  expression to filter on. EX. \"port 80\"\n"
                 "\t-n  number of packets to listen for Ex. 15\n";
    /* Command line args:
     -p port
     */
    while ((o = getopt (argc, argv, "i:p:e:")) != -1) {
        switch(o){
            case 'i':
                dev = optarg;
                break;
            case 'n':
                num_packets = atoi(optarg);
                break;
            case 'e':
                filter_exp = (char*)malloc(sizeof(char)*sizeof(optarg));
                filter_flag = 1;
                break;
            case '?':
                if(optopt == 'i') {
                    fprintf (stderr, "Option %c requires an argument. EX. eth0 \n", optopt);
                    return 0;
                }
                else if(optopt == 'n'){
                    fprintf(stderr,"Option %c requires an argument. Ex. 15\n",optopt);
                    return 0;
                }
                else if(optopt =='e'){
                    fprintf(stderr,"Option %c requires an argument. Ex. \"port 80\"\n",optopt);
                    return 0;
                }
                else {
                    fprintf (stderr, "%s\n", help);
                    return 0;
                }
                break;
        }
    }

    //if no flags set print error message
    if (argc == 1)
    {
        fprintf(stderr,help,optopt);
        return 0;
    }
    if(filter_flag == 0)
    {
        filter_exp = "";
    }


    /*If no device is listed, send an error message*/
    if(dev == NULL){ 
        fprintf(stderr,"Couldn't find default device: %s\n",errbuf);
        return(2);
    }



    /*Print the device message*/
    printf("Device: %s\n",dev);
   

    /*Retrieve network mask and number from capture device*/
    if(pcap_lookupnet(dev,&net, &mask,errbuf) == -1){
        fprintf(stderr,"Couldn't get netmask for device %s: %s\n",dev,errbuf);
        net = 0;
        mask =0;
    }



    /*opens the device to read on. Errors out if failure*/
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if(handle == NULL){
        fprintf(stderr, "Couldn't open device %s: %s\n",dev,errbuf);
        return 2;
    }
    

    /*This compiles the regular expression filter*/
    if(pcap_compile(handle, &fp, filter_exp, 0, net)==-1){
        fprintf(stderr,"Couldn't parse filter %s: %s\n",filter_exp, pcap_geterr(handle));
        return 2;
    }

    /*After compilation this applys the filter*/
    if(pcap_setfilter(handle,&fp) == -1) {
        fprintf(stderr,"Couldn't install filter %s:%s\n",filter_exp,pcap_geterr(handle));
        return 2;
    }

    /*Grab a single packet */
   // packet = pcap_next(handle,&header);
    /*Print its length */
    //printf("Jacked a packet with length of [%d]\n",header.len);


    /*Set up our callback function*/
    pcap_loop(handle,num_packets,got_packet,NULL);


    /*Close the session*/
    pcap_freecode(&fp);
    pcap_close(handle);
    if(filter_flag == 1)
    {
        free(filter_exp);
    }
    printf("\nFinished capturing packet\n");
    return 0;
}
