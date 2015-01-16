#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

/****************************************
        Author: Tim Wood, Chenghu He
        with a little help from
        http://beej.us/guide/bgnet/

****************************************/

int main(int argc, char ** argv)
{
        char* server_port = "1234";
        char* server_ip = "127.0.0.1";
        char *message = "Hello World (from udp)";
        int sockfd, rc;
        struct addrinfo hints, *server;
        int o;

        /* Command line args:
                -p port
                -h host name or IP
        */
        while ((o = getopt (argc, argv, "p:h:m:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        server_ip = optarg;
                        break;
                case 'm':
                        message = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'h' ) {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        printf("server_ip: %s   port: %s\n", server_ip, server_port);

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM; /* or SOCK_STREAM */

        /* getaddrinfo() gives us back a server address we can connect to.
           It actually gives us a linked list of addresses, but we'll just use the first.
         */
        if ((rc = getaddrinfo(server_ip, server_port, &hints, &server)) != 0) {
                perror(gai_strerror(rc));
                exit(-1);
        }

        /* Now we can create the socket and connect */
        sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if (sockfd == -1) {
                perror("ERROR opening socket");
                exit(-1);
        }

        /* The UDP use sendto without connect */
        rc = sendto(sockfd, message, strlen(message)+1, 0, server->ai_addr, server->ai_addrlen);
        if(rc < 0) {
                perror("ERROR on sendto");
                exit(-1);
        }

        out:
        freeaddrinfo(server);
        close(sockfd);

        printf("Done.\n");
        return 0;
}
