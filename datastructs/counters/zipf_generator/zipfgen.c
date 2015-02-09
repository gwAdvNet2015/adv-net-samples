#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <math.h>

/****************************************
        Author: Tim Wood
                Chenghu He
        with a little help from
        http://beej.us/guide/bgnet/
        with a source of zipf generator
        www.csee.usf.edu/~christen/tools/genzipf.c
****************************************/

struct send_info {
        char *addr;
        char *port;
        char *alpha;
        char *nval;
};

int get_zipf_key(double alpha, int n) 
{
        static int first = 1;      // Static first time flag
        static double c = 0;          // Normalization constant
        double z;                     // Uniform random number (0 < z < 1)
        double sum_prob;              // Sum of probabilities
        double zipf_value;            // Computed exponential value to be returned
        int    i;                     // Loop counter


        // Compute normalization constant on first call only
        if (first == 1)
        {
                for (i=1; i<=n; i++)
                c = c + (1.0 / pow((double) i, alpha));
                c = 1.0 / c;
                first = 0;
        }

        // Pull a uniform random number (0 < z < 1)
        do
        {
                z = (double)rand()/((double)RAND_MAX);
        }
        while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i=1; i<=n; i++)
        {
                sum_prob = sum_prob + c / pow((double) i, alpha);
                if (sum_prob >= z)
                {
                        zipf_value = i;
                        break;
                }
        }

        // Assert that zipf_value is between 1 and N
        if ((zipf_value < 1) || (zipf_value > n)) {
                perror("zipf result invalid");
                exit(-1);
        }

        return zipf_value;
}

int send_key_tcp(struct send_info *info)
{
        int sockfd, rc;
        struct addrinfo hints, *server;
        char message[256];
        double Alpha;
        int N;

        Alpha = atof(info->alpha);
        N = atoi(info->nval);
        if (N == 0) {
                perror("input value invalid");
                exit(-1);
        }

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

        while(1) {
                *(int *)message = get_zipf_key(Alpha, N);
                
                /* Send the message, plus the \0 string ending. Use 0 flags. */
                rc = send(sockfd, message, sizeof(int), 0);
                if(rc < 0) {
                        perror("ERROR on send");
                        exit(-1);
                }       
        }

        freeaddrinfo(server);
        close(sockfd);

        return 0;
}

int main(int argc, char ** argv)
{
        char* server_port = "1234";
        char* server_ip = "127.0.0.1";
        char* zipf_alpha = "1.0";
        char* zipf_N = "10";
        int o;
        struct send_info sinfo;

        srand(time(NULL));

        /* Command line args:
                -p port
                -h host name or IP
                -a zipf alpha value (double)
                -n zipf n value (int)
        */
        while ((o = getopt (argc, argv, "p:h:a:n:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        server_ip = optarg;
                        break;
                case 'a':
                        zipf_alpha = optarg;
                        break;
                case 'n':
                        zipf_N = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'h' || optopt == 'a' || optopt == 'n') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        printf("Zipf generator usage: -p [port] -h [host] -a [alpha] -n [N]\n");

        //send key
        sinfo.addr = server_ip;
        sinfo.port = server_port;
        sinfo.alpha = zipf_alpha;
        sinfo.nval = zipf_N;
        if (send_key_tcp(&sinfo) != 0) {
                perror("ERROR on send key");
                exit(-1);
        }

        return 0;
}
