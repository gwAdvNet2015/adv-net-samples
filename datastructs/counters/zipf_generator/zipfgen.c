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
        Author: Tim Wood
                Chenghu He
        with a little help from
        http://beej.us/guide/bgnet/
****************************************/

struct send_info {
        int key;
        char *addr;
        char *port;
};

int get_zipf_key() 
{

        return 0;
}

int send_key_tcp(struct send_info *info)
{
        int sockfd, rc;
        struct addrinfo hints, *server;
        char message[256];

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */

        /* getaddrinfo() gives us back a server address we can connect to.
           It actually gives us a linked list of addresses, but we'll just use the first.
         */
        if ((rc = getaddrinfo(info->addr, info->port, &hints, &server)) != 0) {
                perror(gai_strerror(rc));
                exit(-1);
        }

        /* Now we can create the socket and connect */
        sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if (sockfd == -1) {
                perror("ERROR opening socket");
                exit(-1);
        }
        rc = connect(sockfd, server->ai_addr, server->ai_addrlen);
        if (rc == -1) {
                perror("ERROR on connect");
                close(sockfd);
                exit(-1);
                // TODO: could use goto here for error cleanup
        }

        *(int *)message = info->key;
        /* Send the message, plus the \0 string ending. Use 0 flags. */
        rc = send(sockfd, message, sizeof(int), 0);
        if(rc < 0) {
                perror("ERROR on send");
                exit(-1);
        }

        freeaddrinfo(server);
        close(sockfd);

        return 0;
}

int main(int argc, char ** argv)
{
        char* server_port = "1234";
        char* server_ip = "127.0.0.1";
        int o;
        struct send_info sinfo;

        /* Command line args:
                -p port
                -h host name or IP
        */
        while ((o = getopt (argc, argv, "p:h:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        server_ip = optarg;
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

        //get key

        //send key
        sinfo.key = 0;
        sinfo.addr = server_ip;
        sinfo.port = server_port;
        if (send_key_tcp(&sinfo) != 0) {
                perror("ERROR on send key");
                exit(-1);
        }

        return 0;
}
