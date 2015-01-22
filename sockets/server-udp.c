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

#define BACKLOG 10     // how many pending connections queue will hold


int main(int argc, char ** argv)
{
        char* server_port = "1234";
        int sockfd, rc;
        int yes=1;
        struct addrinfo hints, *server;
        char message[256];
        int o;

        /* Command line args:
                -p port
        */
        while ((o = getopt (argc, argv, "p:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case '?':
                        if(optopt == 'p') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        printf("receiving from port: %s\n", server_port);

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM; /* or SOCK_STREAM */
        hints.ai_flags = AI_PASSIVE;

        /* getaddrinfo() gives us back a server address we can connect to.
           The first parameter is NULL since we want an address on this host.
           It actually gives us a linked list of addresses, but we'll just use the first.
         */
        if ((rc = getaddrinfo(NULL, server_port, &hints, &server)) != 0) {
                perror(gai_strerror(rc));
                exit(-1);
        }

        /* Now we can create the socket and bind it to the local IP and port */
        sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if (sockfd == -1) {
                perror("ERROR opening socket");
                exit(-1);
        }
        /* Get rid of "Address already in use" error messages */
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                perror("setsockopt");
                exit(-1);
        }
        rc = bind(sockfd, server->ai_addr, server->ai_addrlen);
        if (rc == -1) {
                perror("ERROR on connect");
                close(sockfd);
                exit(-1);
                // TODO: could use goto here for error cleanup
        }

        /* Loop forever accepting new connections. */
        while(1) {
                struct sockaddr_storage client_addr;
                socklen_t addr_size;
                int bytes_read;

                addr_size = sizeof client_addr;

                /* The UDP uses recvfrom to receive data with out listen and accept */
                /* Set to flag MSG_WAITALL for blocking the loop of receiving */
                bytes_read = recvfrom(sockfd, message, sizeof message, MSG_WAITALL, (struct sockaddr *)&client_addr, &addr_size); 
                if(bytes_read < 0) {
                        perror("ERROR recvfrom");
                }
                printf("Read: %s\n", message);
        }

        out:
        freeaddrinfo(server);
        close(sockfd);

        printf("Done.\n");
        return 0;
}
