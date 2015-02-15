#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sys/time.h>
#include <sys/errno.h>

/****************************************
        Author: Chenghu He, Tim Wood, Anthony Korzan
        Code based on client-tcp.c and server-tcp.c
        http://beej.us/guide/bgnet/
****************************************/

#define BACKLOG 10     // how many pending connections queue will hold
#define TIMEOUT 500000// time in microseconds to wait to receive response for clients

/*
        server_loop_tcp works as a server using TCP, handles requests, and sends the count numbers back
*/
int server_loop_tcp(char *server_port) 
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
                perror("ERROR on setsockopt");
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

                printf("Connection dropped!\n");
                close(clientfd);
        }

        freeaddrinfo(server);
        close(sockfd);
        return 0;
}

/*
        server_loop_udp listens to udp connections, handles requests, and sends the count numbers back
*/
int server_loop_udp(char *server_port) 
{
        int sockfd, rc;
        int yes = 1;
        struct addrinfo hints, *server;
        char message[256];

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
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
                perror("ERROR on setsockopt");
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
                int clientfd;
                int bytes_read, bytes_write;

                addr_size = sizeof client_addr;

                /* Use a loop to receive couters and reply them */
                while (1) {
                        bytes_read = recvfrom(sockfd, message, sizeof message, MSG_WAITALL, (struct sockaddr *)&client_addr, &addr_size); 
                        if(bytes_read < 0) {
                                perror("ERROR reading socket");
                        }
                        if (bytes_read == 0) break;
                        bytes_write = sendto(sockfd, (char *)&message, bytes_read, 0, (struct sockaddr *)&client_addr, addr_size);
                        if(bytes_write < 0) {
                                perror("ERROR writing socket");
                        }
                }

                printf("Connection dropped!\n");
                close(clientfd);
        }

        freeaddrinfo(server);
        close(sockfd);
        return 0;
}

/*
        Client_count_tcp works to generate count numbers and sends them to the server, 
        reveives the send-back numbers, and calculates the min, max, and average delay
*/
int client_count_tcp(char *server_ip, char *server_port, char *number_input)
{
        int sockfd, rc;
        struct addrinfo hints, *server;
        int number_try, counter = 0, re_counter, dropped = 0, successes = 0;
        struct timeval tval_start, tval_end, sock_opts;
        long long int diff, min = -1LL, max = -1LL;
        long double avg = 0.0L;

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

        /* Set a timeout option of 500 miliseconds on the socket */
        sock_opts.tv_sec = 0;
        sock_opts.tv_usec = TIMEOUT;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &sock_opts, sizeof(sock_opts)) < 0) {
                perror("ERROR on setsockopt");
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
        while (counter < number_try) {
                /* A boolean */
                int skip = 0;
                /* Start of the time calculation */
                gettimeofday(&tval_start, NULL);

                /* The send and recv circle */
                rc = send(sockfd, (char *)&counter, sizeof counter, 0);
                if(rc < 0) {
                        perror("ERROR on send");
                        exit(-1);
                }
                /* We have just sent a message, increment the counter */
                counter++;

                /* We may have prematurely timed-out and our packet could be waiting for us.
                 * We need to make sure the packet we recv is intended for this itteration and not the
                 * previous one.  Thus, the we can simply check if the replied counter matches. */
                do {
                        /* Set to flag MSG_WAITALL for blocking the loop of receiving */
                        rc = recv(sockfd, (char *)&re_counter, sizeof re_counter, 0);
                        
                        if(rc < 0) {
                                /* Packet timed-out */
                                if (errno == EAGAIN) {
                                        skip = -1;
                                } else {
                                        perror("ERROR on recv");
                                        exit(-1);
                                }
                        }
                } while (re_counter != counter - 1 || skip);
                /* A packet timed-out, skip this iteration. */
                if (skip) {
                        dropped++;
                        continue;
                }
                /* At this point we received a correct response! */
                successes++;

                /* End of the time calculation */
                gettimeofday(&tval_end, NULL);

                /* Calculation */
                diff = (tval_end.tv_sec - tval_start.tv_sec) * 1000000LL + tval_end.tv_usec - tval_start.tv_usec;
                if (min == -1LL || diff < min) min = diff;  
                if (max == -1LL || diff > max) max = diff;
                avg = avg * (successes - 1) / successes + (long double)diff / successes ;
        }

        /* Show the result */
        printf("Result:\n");
        printf("        The total number of loops   : %d\n", number_try);
        printf("        The min time of latency     : %lld us\n", min);
        printf("        The max time of latency     : %lld us\n", max);
        printf("        The average time of latency : %Lf us\n", avg);
        printf("        Number of Packets Dropped   : %d\n", dropped);

        freeaddrinfo(server);
        close(sockfd);
        return 0;
}

/*
        Same functionality as client_count_tcp but uses UDP and measures dropped pockets.
*/
int client_count_udp(char *server_ip, char *server_port, char *number_input)
{
        int sockfd, rc;
        struct addrinfo hints, *server;
        int number_try, counter = 0, re_counter, dropped = 0, successes = 0;
        struct timeval tval_start, tval_end, sock_opts;
        long long int diff, min = -1LL, max = -1LL;
        long double avg = 0.0L;

        /* Get the number from string number_input and check the result */
        number_try = atoi(number_input);
        if (number_try < 1) {
                printf("Invalid number input, number of connections resets to 100\n");
                number_try = 100;
        }        

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

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

        /* Set a timeout option of 500 miliseconds on the socket */
        sock_opts.tv_sec = 0;
        sock_opts.tv_usec = TIMEOUT;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &sock_opts, sizeof(sock_opts)) < 0) {
                perror("ERROR on setsockopt");
                exit(-1);
        }

        /* Send the message with a loop */
        while (counter < number_try) {
                /* Boolean */
                int skip = 0;
                /* We will need to allow the server to reach us back by UDP,
                   and this is where we store the server's info connecting as a client to us. */
                struct sockaddr_storage client_addr;
                socklen_t addr_size = sizeof client_addr;
                /* Start of the time calculation */
                gettimeofday(&tval_start, NULL);

                /* Sendto using UDP as specified in sockfd by server->ai_protocol */
                rc = sendto(sockfd, (char *)&counter, sizeof counter, 0, server->ai_addr, server->ai_addrlen);
                if(rc < 0) {
                        perror("ERROR on sendto");
                        exit(-1);
                }
                /* We have just sent a message, increment the counter */
                counter++;

                /* We may have prematurely timed-out and our packet could be waiting for us.
                 * We need to make sure the packet we recv is intended for this itteration and not the
                 * previous one.  Thus, the we can simply check if the replied counter matches. */
                do {
                        /* Set to flag MSG_WAITALL for blocking the loop of receiving */
                        rc = recvfrom(sockfd, (char *)&re_counter, sizeof re_counter, MSG_WAITALL, (struct sockaddr *)&client_addr, &addr_size);
                        
                        if(rc < 0) {
                                /* Packet timed-out */
                                if (errno == EAGAIN) {
                                        skip = -1;
                                } else {
                                        perror("ERROR on recvfrom");
                                        exit(-1);
                                }
                        }
                } while (re_counter != counter - 1 || skip);
                /* A packet timed-out, skip this iteration. */
                if (skip) {
                        dropped++;
                        continue;
                }
                /* At this point we received a correct response! */
                successes++;

                /* End of the time calculation */
                gettimeofday(&tval_end, NULL);

                /* Calculation */
                diff = (tval_end.tv_sec - tval_start.tv_sec) * 1000000LL + tval_end.tv_usec - tval_start.tv_usec;
                if (min == -1LL || diff < min) min = diff;  
                if (max == -1LL || diff > max) max = diff;
                avg = avg * (successes - 1) / successes + (long double)diff / successes ;
        }

        /* Show the result */
        printf("Result:\n");
        printf("        The total number of loops   : %d\n", number_try);
        printf("        The min time of latency     : %lld us\n", min);
        printf("        The max time of latency     : %lld us\n", max);
        printf("        The average time of latency : %Lf us\n", avg);
        printf("        Number of Packets Dropped   : %d\n", dropped);

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
        char protocol = 't';
        int o;

        /* Command line args:
                -n times of loop (default is 100)
                -m mode client or server
                -p port
                -h host name or IP
                -s socket (UDP or TCP)
        */
        while ((o = getopt (argc, argv, "p:h:m:n:s:")) != -1) {
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
                case 's':
                        if (optarg[0] == 'T' || optarg[0] == 't') {
                                protocol = 't';
                        } else if (optarg[0] == 'U' || optarg[0] == 'u') {
                                protocol = 'u';
                        } else {
                                fprintf (stderr, "Option %c accepts either TCP or UDP\n", optopt);
                        }
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
                        switch (protocol) {
                        case 't': // TCP
                                printf("        Server's Protocol : TCP\n");
                                server_loop_tcp(server_port);
                                break;
                        case 'u': // UDP
                                printf("        Server's Protocol : UDP\n");
                                server_loop_udp(server_port);
                                break;
                        }
                        return 0;
                }
                else if (strncmp(mode, "client", 6) == 0) {
                        printf("LatencyCount[%d] works on client mode!\n", getpid());
                        printf("        Remote Server IP  : %s\n", server_ip);
                        printf("        Remote Server Port: %s\n", server_port);
                        printf("        Connecting Times  : %s\n", number_input);
                        switch (protocol) {
                        case 't': // TCP
                                printf("        Client's Protocol : TCP\n");
                                client_count_tcp(server_ip, server_port, number_input);
                                break;
                        case 'u': // UDP
                                printf("        Client's Protocol : UDP\n");
                                client_count_udp(server_ip, server_port, number_input);
                                break;
                        }
                        return 0;
                }        
        }

        /* Error message */
        printf("LatencyCount[%d] doesn't work. Please choose a mode!\n", getpid());
        printf("Usage:\n        -m client or server\n        -h server's ip\n        -p port\n        -n times of loop\n");

        return 0;
}

