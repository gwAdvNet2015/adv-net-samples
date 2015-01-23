#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc , char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rc, ctr;
	
	//if argc size is less than 2, then the hostname was not included as a command line input.	
	if(argc <3){
		printf("Error: Missing Hostname/IP Address or flag to decide IP->Hostname or Hostname->IP\n");
		exit(1);
	}
	//if command line input is "-h", then hostname was given.
	if (strcmp(argv[1], "-h") == 0) {
		char *hostname = argv[2]; //hostname from command line input.
    		char ip[100]; 		  //set up to take in the IPv4 address later.

    		memset(&hints, 0, sizeof hints); //setting space for hints.
    		hints.ai_family = AF_INET;	 //setting the family type to IPv4 only.
    		hints.ai_socktype = SOCK_STREAM; //setting socket type to streams.
    
    		//if the return value is not zero, there is an error.
    		if((rc = getaddrinfo(hostname, "http", &hints, &servinfo)) != 0)
    		{
    			perror(gai_strerror(rc)); //print the error. gai_strerror translates the error to readable human text.
    			exit(-1);
    		}
    	
        	ctr = 1;
    		//find the first connection from the results
    		for(p = servinfo; p != NULL; p = p->ai_next)
    		{
    			h = (struct sockaddr_in *) p->ai_addr; //set h to the pointer of the sockaddr struct. ai_addr is a pointer to a filled-in socket address struct.
    			strcpy(ip, inet_ntoa(h->sin_addr)); //sin_addr holds the IPv4 address. this is copied to ip.
        	    printf("%s - IPv4 address #%d: %s\n" ,hostname , ctr, ip);
        	    ctr++;
    		}

    	}
    	//otherwise, if command line input is "-i", then IPv4 address was given.
	else if(strcmp(argv[1], "-i") == 0){
		struct addrinfo *res=0; //Setting addrinfo struct for getaddrinfo.
		int rc;             //used for error-checking later on.
        
        	char *ip = argv[2];     //command line input
    		char hostname[1000];     //variable to store output
        	printf("Command line input: %s\n", ip); //output to show user their input.
        
        	rc = getaddrinfo(ip, 0, 0, &res);   //getaddrinfo is used to supply parameters to getnameinfo.
        	//error checking getaddrinfo.
        	if(rc != 0)
    		{
    			perror(gai_strerror(rc)); //print the error. gai_strerror translates the error to readable human text.
    			exit(-1);
    		}
    	
		rc = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, 1000, 0, 0, 0);
        	//error checking getnameinfo.
        	if(rc != 0)
    		{
    			perror(gai_strerror(rc)); //print the error. gai_strerror translates the error to readable human text.
    			exit(-1);
    		}
        	printf("Hostname found: %s\n", hostname);
    	}
}
