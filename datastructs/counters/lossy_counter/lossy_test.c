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

//show result and free memory
void show_result(Counter *result) {
        int len = 0;
        int i;

        len = result[0].item;
        printf("Item            Count\n");

        for (i = 1; i <= len; i++) {
                printf("%-16d%-16d\n", result[i].item, result[i].count);
        }   
}

void counter_test(char *lossy_phi, char *zipf_N, char *zipf_alpha, char *loops, char *outrate) {
        LC_type *lcounter;
        Counter * result;
        int i, key;
        double alpha, phi;
        int N, R, L;

        phi = atof(lossy_phi);
        if (phi == 0) {
                perror("lossy phi value invalid");
                exit(-1);
        }
        lcounter = LC_Init(phi);
        alpha = atof(zipf_alpha);
        N = atoi(zipf_N);
        if (N == 0) {
                perror("zipf N value invalid");
                exit(-1);
        }   
        L = atoi(loops);
        R = atoi(outrate);
        if (R == 0) {
                perror("outrate value invalid");
                exit(-1);
        }
        printf("        Lossy phi :     %f\n", phi); 
        printf("        zipf N :        %d\n", N); 
        printf("        zipf alpha :    %f\n", alpha); 
        printf("        loops :         %d\n", L); 
        printf("        output period : %d\n", R); 

        for (i = 0; i < L; i++) {
                key = get_zipf_key(alpha, N); 
                LC_Update(lcounter, key);
                if (i % R == 0) {
                        result = LC_Output(lcounter, 1);
                        show_result(result);
                }
        }

        return;
}

int main(int argc, char ** argv)
{
        char* phi = "0.01";
        char* N = "1000";
        char* alpha = "0.5";
        char* loops = "10000";
        char* outrate = "1000";
        int o;

        /* Command line args: */
        printf("Lossy speed tester usage: -p [lossy phi] -n [zipf N] -a [zipf alpha] -l [loops] -r [output period]\n");

        while ((o = getopt (argc, argv, "p:n:a:l:r:")) != -1) {
                switch(o){
                case 'p':
                        phi = optarg;
                        break;
                case 'n':
                        N = optarg;
                        break;
                case 'a':
                        alpha = optarg;
                        break;
                case 'l':
                        loops = optarg;
                        break;
                case 'r':
                        outrate = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'n' || optopt == 'a' || optopt == 'l') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        counter_test(phi, N, alpha, loops, outrate); 

        return 0;
}

