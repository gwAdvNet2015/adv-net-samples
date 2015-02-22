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

#include "zipfgen.h"

/****************************************
        Author: Tim Wood
                Chenghu He
        with a little help from
        http://beej.us/guide/bgnet/
        with a source of zipf generator
        www.csee.usf.edu/~christen/tools/genzipf.c
****************************************/

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

