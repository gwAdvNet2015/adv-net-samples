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
        Author: Phil Lopreiato
        Author: Neel Shah
        with a little help from
        http://beej.us/guide/bgnet/
****************************************/

#define MAX_PAYLOAD_SIZE 1024
#define DEFAULT_NUM_PACKETS -1 /* Go forever */
#define DEFAULT_NUM_THREADS 1 /* Make one thread and use it */
#define DEFAULT_SEND_DELAY 0 /* Lightspeed is too slow,
                                we'll have to go right to ludicrous speed! */
#define DEFAULT_NUM_THREADS 1 /* Make one worker thread */
#define DEFAULT_OUT_TIME 1 /* Output info every second */

/*
 * Struct contains arguments for the thread handler function
 */
struct arguments {
        int num_pkts;
        struct addrinfo server;
        char *msg;
        int send_delay;
};

int pkt_total;
struct arguments args;
pthread_mutex_t send_lock = PTHREAD_MUTEX_INITIALIZER,
                time_lock = PTHREAD_MUTEX_INITIALIZER;
int time_thread_die_cond;

void*
handle_thread_time(void *args)
{
        int out_time = *((int*)(&args));

        while (1) {
                sleep(out_time);
                pthread_mutex_lock(&time_lock);
                printf("Number pkts sent: %d\n", pkt_total);
                if (time_thread_die_cond == 0) break;
                pthread_mutex_unlock(&time_lock);
        }

        pthread_exit(NULL);
}

void*
handle_thread_send(void *arg)
{
        printf("New thread created with id %d\n", pthread_self());

        int i, sockfd, rc, num_pkts, delay;
        struct addrinfo server;
        char *msg;

        pthread_mutex_lock(&send_lock);
        num_pkts = args.num_pkts;
        server = args.server;
        msg = args.msg;
        delay = args.send_delay;
        pthread_mutex_unlock(&send_lock);

        sockfd = socket(server.ai_family, server.ai_socktype, server.ai_protocol);
        if (sockfd == -1) {
                perror("ERROR opening socket");
                exit(-1);
        }

        rc = connect(sockfd, server.ai_addr, server.ai_addrlen);
        if (rc == -1) {
                perror("ERROR on connect");
                goto thread_out;
        }

        /* Send messages as fast as possible
         * Use infinite while/break to allow for infinite sending (avoid overflow with for counters)
         */
        i = 0;
        while(1){
                /* Send the message, plus the \0 string ending. Use 0 flags. */
                rc = send(sockfd, msg, MAX_PAYLOAD_SIZE, 0);
                if (rc < 0) {
                        perror("ERROR on send");
                        exit(-1);
                }

                pthread_mutex_lock(&send_lock);
                pkt_total++;
                pthread_mutex_unlock(&send_lock);

                /* Only break if we're not sending infinitely (avoid int overflow issues) */
                if (num_pkts > 0 && i == num_pkts){
                        break;
                }
                i++;

                if (delay > 0){
                        sleep(delay);
                }
        }

        thread_out:
        close(sockfd);
        printf("[%d] Sending complete!\n", pthread_self());

        pthread_exit(NULL);
}

int main(int argc, char ** argv)
{
        char* server_port = "1234";
        char* server_ip = "127.0.0.1";
        char *message = malloc(MAX_PAYLOAD_SIZE);
        int sockfd, rc;
        int i, o, j;
        int num_pkts = DEFAULT_NUM_PACKETS,
            num_thrds = DEFAULT_NUM_THREADS,
            out_time = DEFAULT_OUT_TIME,
            send_delay = DEFAULT_SEND_DELAY;
        struct addrinfo hints, *server;
        pthread_t output_time;

        pkt_total = 0;
        time_thread_die_cond = -1;

        /* Command line args:
                -p port
                -h host name or IP
                -m message to send
                -n number of packets to send
                -t number of threads to create
                -d seconds to wait in between packet sends
                -x amount of seconds between output
        */
        while ((o = getopt (argc, argv, "p:h:m:n:t:x:d:")) != -1) {
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
                case 'n':
                        num_pkts = atoi(optarg);
                        break;
                case 't':
                        num_thrds = atoi(optarg);
                        break;
                case 'x':
                        out_time = atoi(optarg);
                        break;
                case 'd':
                        send_delay = atoi(optarg);
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

        if (num_pkts < 0) {
                printf("Sending packets forever\n");
        } else {
                printf("Sending %d packets\n", num_pkts);
        }

        printf("Connecting to %s:%s\n", server_ip, server_port);

        /* Initialize the message with some data
         * and ensure the string is terminated with \0
         */
        for(i=strlen(message); i<MAX_PAYLOAD_SIZE; i++){
                message[i] = 'A';
        }
        message[MAX_PAYLOAD_SIZE - 1] = '\0';

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

        /* Time to thread the needle */

        /* Set up connection args so child threads know what to do */
        args.num_pkts = num_pkts;
        args.server = *(struct addrinfo *)server;
        args.msg = message;
        args.send_delay = send_delay;

        /* Thread just for outputting total pkts sent ever X seconds */
        if (pthread_create(&output_time, NULL, (void *)handle_thread_time, (void *)out_time) != 0) {
                perror("ERROR creating time thread");
                exit(-1);
        }

        /* Create individual worker threads */
        printf("Creating %d worker threads\n", num_thrds);
        pthread_t clients[num_thrds];
        for (j = 0; j < num_thrds; j++) {
                if (pthread_create(&clients[j], NULL, (void *)handle_thread_send, NULL) != 0) {
                        perror("ERROR creating client thread");
                        exit(-1);
                }
        }

        /* Now, wait for all the threads to finish. Maybe */
        for (j = 0; j < num_thrds; j++) {
                pthread_join(clients[j], NULL);
        }

        pthread_mutex_lock(&time_lock);
        time_thread_die_cond = 0;
        pthread_mutex_unlock(&time_lock);

        printf("Done -- sent %d packets.\n", pkt_total);
        return 0;
}
