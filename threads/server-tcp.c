#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>

/****************************************
        Author: Tim Wood
        Co-Sub-Authors: Eric Armbrust, Neel Shah, Phil Lopreiato
        with a little help from
        http://beej.us/guide/bgnet/
        See 'man pthreads' for more info,
        must be compiled with -lpthreads
****************************************/

#define BACKLOG 10     // how many pending connections queue will hold

/*
 * Max number of concurrent threads.
 * Don't have more than this or bad things *will* happen :c
 * If more than max_clients is reached for concurrency,
 * the server will not be able to serve all clients.
 *
 * FIX: add queue for unserved requests to be processed as
 * threads become avaiable. 
 */
#define MAX_CLIENTS 512

/****************************************
 * Func passed to pthread_create that handles
 * the print off post-connection. If more threads
 * than MAX_CLIENTS call this function, it esplodes
 * ...don't do it. Serioudsly
 ****************************************/
void
*handle_client(void *arg)
{
        int clientfd = *((int*)(&arg)), bytes_read = -1;
        char message[256];

        printf("Thread %08x\n returning to memory heaven.\n", pthread_self());
        while(bytes_read){
                bytes_read = read(clientfd, message, sizeof message);
                if(bytes_read < 0) {
                        perror("ERROR reading socket");
                        break;
                }else if(bytes_read > 0){
                        printf("[%d] Read: %s\n", pthread_self(), message);
                }
        }

        close(clientfd);
        pthread_exit(NULL);
}

int main(int argc, char ** argv)
{
        int sockfd, rc, yes = 1, o, i = 0;
        char message[256];
        char* server_port = "1234";
        pthread_t client[MAX_CLIENTS];
        struct addrinfo hints, *server;

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

        printf("listening on port: %s\n", server_port);

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */
        hints.ai_flags = AI_PASSIVE;

        /* getaddrinfo() gives us back a server address we can connect to.
           The first parameter is NULL since we want an address on this host.
           It actually gives us a linked list of addresses, but we'll just use the first.
         */
        if (rc = getaddrinfo(NULL, server_port, &hints, &server) != 0) {
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

        /* Time to listen for clients.*/
        listen(sockfd, BACKLOG);
        /* Loop forever accepting new connections. */
        while(1) {
                int clientfd;
                socklen_t addr_size;
                struct sockaddr_storage client_addr;

                i++;
                i = i % MAX_CLIENTS;

                addr_size = sizeof client_addr;
                clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

                /* Eric: Create a thread th handle read after server accepts client socket. */
                pthread_create(&client[i], NULL, (void *)handle_client, (void *)(intptr_t)clientfd);
        }

        out:
        freeaddrinfo(server);
        close(sockfd);

        for(i = 0; i < MAX_CLIENTS; i++) pthread_join(client[i], NULL);

        printf("Done.\n");
        return 0;
}
