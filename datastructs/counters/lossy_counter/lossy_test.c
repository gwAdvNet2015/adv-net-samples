#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>

#include "lossycon.h"
#include "zipfgen.h"

int main(int argc, char ** argv)
{
        char* server_port = "1234";
        char* lossy_phi = "0.01";
        int o;

        /* Command line args:
                -p port
                -i phi for lossy counting
        */
        while ((o = getopt (argc, argv, "p:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'i':
                        lossy_phi = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'i') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        printf("Lossy counter usage: -p [port] -i [phi]\n");

        if (recv_key_tcp(server_port, lossy_phi) != 0) {
                perror("Error on recv key by tcp");
                exit(-1);
        }

        return 0;
}

