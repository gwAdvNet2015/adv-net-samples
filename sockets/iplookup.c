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
	int rc;
	
	//if argc size is less than 2, then the hostname was not included as a command line input.	
	if(argc <2){
		printf("Error: Missing Hostname.");
		exit(1);
	}

	char *hostname = argv[1]; //hostname from command line input.
	char ip[16]; 		  //set up to take in the IPv4 address later.

	memset(&hints, 0, sizeof hints); //setting space for hints.
	hints.ai_family = AF_INET;	 //setting the family type to IPv4 only.
	hints.ai_socktype = SOCK_STREAM; //setting socket type to streams.

	//if the return value is not zero, there is an error.
	if((rc = getaddrinfo(hostname,"http",&hints,&servinfo)) != 0)
	{
		perror(gai_strerror(rc)); //print the error. gai_strerror translates the error to readable human text.
		exit(-1);
	}

	//find the first connection from the results
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		h = (struct sockaddr_in *) p->ai_addr; //set h to the pointer of the sockaddr struct. ai_addr is a pointer to a filled-in socket address struct.
		strcpy(ip, inet_ntoa(h->sin_addr)); //sin_addr holds the IPv4 address. this is copied to ip.
	}

	printf("The IPv4 address of %s is %s" , hostname , ip);

	printf("\n");
}
