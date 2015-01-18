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
        Author: Chenghu He, Tim Wood
        Code based on client-tcp.c and server-tcp.c
        http://beej.us/guide/bgnet/
****************************************/

#define BACKLOG 10     // how many pending connections queue will hold

/*
        The server_loop works as a server, handle requests and sends the count numbers back
*/
int server_loop(char *server_port) 
{
        int sockfd, rc;
        int yes = 1;
        struct addrinfo hints, *server;
        char message[256];

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */
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

        /* Time to listen for clients.*/
        listen(sockfd, BACKLOG);
        /* Loop forever accepting new connections. */
        while(1) {
                struct sockaddr_storage client_addr;
                socklen_t addr_size;
                int clientfd;
                int bytes_read, bytes_write;

                addr_size = sizeof client_addr;
                clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

                /* Use a loop to receive couters and reply them */
                while (1) {
                        bytes_read = read(clientfd, message, sizeof message);
                        if(bytes_read < 0) {
                                perror("ERROR reading socket");
                        }
                        if (bytes_read == 0) break;
                        bytes_write = write(clientfd, message, bytes_read);
                        if(bytes_write < 0) {
                                perror("ERROR writing socket");
                        }
                }

                printf("Connection droped!\n");
                close(clientfd);
        }

        freeaddrinfo(server);
        close(sockfd);
        return 0;
}

/*
        The client_count works for generate count numbers and sends them to the server, 
        reveives the send-back numbers, and calculates the min, max, and average delay
*/
int client_count(char *server_ip, char *server_port, char *number_input)
{
        int sockfd, rc;
        struct addrinfo hints, *server;
        int number_try, counter = 1, re_counter;

        /* Get the number from string number_input and check the result */
        number_try = atoi(number_input);
        if (number_try < 1) {
                printf("Invalid number input, number of connections resets to 100\n");
                number_try = 100;
        }        

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */

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
        rc = connect(sockfd, server->ai_addr, server->ai_addrlen);
        if (rc == -1) {
                perror("ERROR on connect");
                close(sockfd);
                exit(-1);
                // TODO: could use goto here for error cleanup
        }

        /* Send the message with a loop */
        while (counter <= number_try) {
                rc = send(sockfd, (char *)&counter, sizeof counter, 0);
                if(rc < 0) {
                        perror("ERROR on send");
                        exit(-1);
                }
                rc = recv(sockfd, (char *)&re_counter, sizeof re_counter, 0);
                if(rc < 0) {
                        perror("ERROR on recv");
                        exit(-1);
                }
                printf("number %d sent, result is %s \n", counter, (counter == re_counter)?"Equal":"NotEqual");
                counter++;
        }

        freeaddrinfo(server);
        close(sockfd);
        return 0;
}

/*
        The main function is used to get the argument and choose correct mode and parameters
*/
int main(int argc, char ** argv)
{
        char *mode = "";
        char *server_port = "1234";
        char *server_ip = "127.0.0.1";
        char *number_input = "100";
        int o;

        /* Command line args:
                -n times of loop (default is 100)
                -m mode client or server
                -p port
                -h host name or IP
        */
        while ((o = getopt (argc, argv, "p:h:m:n:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        server_ip = optarg;
                        break;
                case 'm':
                        mode = optarg;
                        break;
                case 'n':
                        number_input = optarg;
                        break;
                case '?':
                        if(optopt == 'm' || optopt == 'p' || optopt == 'h' ) {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        /* Select the working mode */
        if (strlen(mode) == 6) {
                if (strncmp(mode, "server", 6) == 0) {
                        printf("LatencyCount[%d] works on server mode!\n", getpid());
                        printf("        Local Server Port : %s\n", server_port);
                        server_loop(server_port);
                        return 0;
                }
                else if (strncmp(mode, "client", 6) == 0) {
                        printf("LatencyCount[%d] works on client mode!\n", getpid());
                        printf("        Remote Server IP  : %s\n", server_ip);
                        printf("        Remote Server Port: %s\n", server_port);
                        printf("        Connecting Times  : %s\n", number_input);
                        client_count(server_ip, server_port, number_input);
                        return 0;
                }        
        }

        /* Error message */
        printf("LatencyCount[%d] doesn't work. Please choose a mode!\n", getpid());
        printf("Usage:\n        -m client or server\n        -h server's ip\n        -p port\n        -n times of loop\n");

        return 0;
}

