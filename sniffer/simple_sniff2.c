//
//  simple_sniff2.c
//  
//
//  Created by lucas on 2/8/15.
//
//

#include <stdio.h>
#include <pcap.h> //This is the important library
#include "simple_sniff2.h"

int main(int argc,char *argv[])
{

    /*This is the device to sniff on and the error string*/
    char *dev = "eth0", errbuf[PCAP_ERRBUF_SIZE];

    /*This the session handler*/
    pcap_t *handle;

    /*The location of the compiled filter expression*/
    struct bpf_program fp;

    /*The filter expression*/
    char filter_exp[] = "port 80";

    /*The netmask of our sniffing device*/
    bpf_u_int32 mask;

    /*This holds the ip address for the sniffing device*/
    bpf_u_int32 net;

    /*This is the header that pcap gives us*/
    struct pcap_pkthdr header;

    /*The actual packet we receive*/
    const u_char *packet;


    /*If no device is listed, send an error message*/
    if(dev == NULL){ 
        fprintf(stderr,"Couldn't find default device: %s\n",errbuf);
        return(2);
    }

    /*Print the error message*/
    printf("Device: %s\n",dev);
    
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

    /*Grab a packet */
    packet = pcap_next(handle,&header);
    
    /*Print its length */
    printf("Jacked a packet with length of [%d]\n",header.len);

    /*Close the session*/
    pcap_close(handle);

    return 0;
}
