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
#include <sys/time.h>
#include <signal.h>

#include "lossycon.h"
#include "zipfgen.h"
/****************************************
        Author: Tim Wood
                Chenghu He
        
        with a little help from
        http://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
        http://www.gnu.org/software/libc/manual/html_node/Handler-Returns.html#Handler-Returns
****************************************/

int _running = 1;

void timeout(int sig) {
        _running = 0;
        signal(sig, timeout);
}

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

void counter_test(char *lossy_phi, char *zipf_N, char *zipf_alpha, 
                        char *runs, char *outrate, int debug, char *loops) {
        LC_type *lcounter;
        Counter *result = NULL;
        int i, key;
        double alpha, phi;
        int N, T, R;
        long long count = 0, L;
        struct itimerval toset, pre;
        struct timeval tval_start, tval_end;
        double counttime = 0;

        phi = atof(lossy_phi);
        if (phi == 0) {
                perror("invalid input");
                exit(-1);
        }
        lcounter = LC_Init(phi);
        alpha = atof(zipf_alpha);
        N = atoi(zipf_N);
        R = atoi(outrate);
        if (N == 0) {
                perror("invalid input");
                exit(-1);
        }   
        T = atoi(runs);
        L = atoll(loops);
        if (L > 0) T = 0;
        
        if (T > 0) {
                signal(SIGALRM, timeout);
                toset.it_interval.tv_usec = 0;
                toset.it_interval.tv_sec = 0;
                toset.it_value.tv_usec = 0;
                toset.it_value.tv_sec = (long int)T;
                setitimer(ITIMER_REAL, &toset, &pre);
        }

        gettimeofday(&tval_start, NULL);

        while(_running == 1) {
                count++;
                key = get_zipf_key(alpha, N); 
                LC_Update(lcounter, key);
                if (R > 0 && count % R == 0) {
                        if (result != NULL) {
                                free(result);
                                result = NULL;
                        }
                        result = LC_Output(lcounter, 1);
                        if (debug == 1) show_result(result);
                }
                if (T == 0 && count >= L) break; 
        }
        if (R == 0 && debug == 1) {
                if (result != NULL) {
                        free(result);
                        result = NULL;
                }
                result = LC_Output(lcounter, 1);
                show_result(result);
        }

        gettimeofday(&tval_end, NULL);
        counttime = (tval_end.tv_sec - tval_start.tv_sec) + 0.0000001 * (tval_end.tv_usec - tval_start.tv_usec);

        if (debug == 1) {
                printf("Lossy speed tester parameters:\n");
                printf("        Lossy phi :     %f\n", phi); 
                printf("        zipf N :        %d\n", N); 
                printf("        zipf alpha :    %f\n", alpha); 
                printf("        running time :  %f\n", counttime); 
                printf("        output period : %d\n", R); 
                printf("        total count:    %lld\n", count);
                printf("        counts per sec: "); 
        }
        printf("%f\n", (double)count / counttime);
        
        return;
}

void show_help(void) {
        /* Command line args: */
        printf("Lossy speed tester usage: \n");
        printf("        -l [loops] -p [lossy phi] -n [zipf N] -a [zipf alpha]\n");
        printf("        -t [running seconds] -v \"show results\" -r [output period]\n");
        printf("        -h \"show help\"\n");

}

int main(int argc, char ** argv)
{
        char* phi = "0.01";
        char* N = "1000";
        char* alpha = "0.5";
        char* runs = "2";
        char* outrate = "100";
        char* loops = "0";
        int debug = 0;
        int o;

        while ((o = getopt (argc, argv, "p:n:a:t:vr:hl:")) != -1) {
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
                case 't':
                        runs = optarg;
                        break;
                case 'v':
                        debug = 1;
                        break;
                case 'r':
                        outrate = optarg;
                        break;
                case 'h':
                        show_help();
                        return 0;
                case 'l':
                        loops = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'n' || optopt == 'a' 
                                || optopt == 't' || optopt == 'r' || optopt == 'l') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        counter_test(phi, N, alpha, runs, outrate, debug, loops);

        return 0;
}

