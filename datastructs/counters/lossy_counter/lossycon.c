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
/****************************************
        Author: Tim Wood
                Chenghu He
        
        with a little help from
        http://beej.us/guide/bgnet/
        
        with implementation of Lossy Counting
        see Manku & Motwani, VLDB 2002 for details
        implementation by Graham Cormode, 2002,2003
        http://www.cs.rutgers.edu/~muthu/lossycount.c
        This work is licensed under the Creative Commons
        Attribution-NonCommercial License. To view a copy of this license,
        visit http://creativecommons.org/licenses/by-nc/1.0/ or send a letter
        to Creative Commons, 559 Nathan Abbott Way, Stanford, California
        94305, USA. 
****************************************/

// LC implementation
LC_type * LC_Init(float phi)
{
        LC_type * result;

        result=(LC_type *) calloc(1,sizeof(LC_type));
        result->buckets=0;
        result->holdersize=0;
        result->epoch=0;

        result->window=(int) 1.0/phi;
        result->maxholder=result->window*4;  
        result->bucket=(Counter*) calloc(result->window+2,sizeof(Counter));
        result->holder=(Counter*) calloc(result->maxholder,sizeof(Counter));
        result->newcount=(Counter*) calloc(result->maxholder,sizeof(Counter));
        return result;
}

void LC_Delete(LC_type * lc)
{
        free(lc->bucket);
        free(lc->holder);
        free(lc->newcount);
        free(lc);
}

void countershell(unsigned long n, Counter a[])
{
        unsigned long i,j,inc;
        Counter v;
        inc=1;
        do {
                inc *= 3;
                inc++;
        } while (inc <= n);
        do { 
                inc /= 3;
                for (i=inc+1;i<=n;i++) { 
                        v=a[i-1];
                        j=i;
                        while (a[j-inc-1].item > v.item) {
                                a[j-1]=a[j-inc-1];
                                j -= inc;
                                if (j < inc) break;
                        }
                        a[j-1]=v;
                }
        } while (inc > 1);
}

int countermerge(Counter *newcount, Counter *left, Counter *right, 
                      int l, int r, int maxholder) 
{  
        // merge up two lists of counters. returns the size of the lists. 
        int i,j,m;

        if (l+r>maxholder)
        { // a more advanced implementation would do a realloc here...
                printf("Out of memory -- trying to allocate %d counters\n",l+r);
                exit(1);
        }      
        i=0;
        j=0;
        m=0;
  
        while (i<l && j<r)
        { // merge two lists
                if (left[i].item==right[j].item) 
                { // sum the counts of identical items
                        newcount[m].item=left[i].item;
                        newcount[m].count=right[j].count;
                        while (left[i].item==right[j].item)
                        {
                                newcount[m].count+=left[i].count;
                                i++;
                        }
                        j++;
                }                      
                else if (left[i].item<right[j].item) 
                { // else take the left item, creating counts appropriately
                        newcount[m].item=left[i].item;
                        newcount[m].count=0;
                        while (left[i].item==newcount[m].item)
                        {
                                newcount[m].count+=left[i].count;
                                i++;
                        }
                }
                else {
                        newcount[m].item=right[j].item;
                        newcount[m].count=right[j].count;
                        j++;
                }
                newcount[m].count--;
                if (newcount[m].count>0) m++;
                else 
                { // adjust for items which may have negative or zero counts
                        newcount[m].item=-1;
                        newcount[m].count=0;
                }
        }

        // now that the main part of the merging has been done
        // need to copy over what remains of whichever list is not used up

        if (j<r)
        { 
                while (j<r) 
                {
                        if (right[j].count > 1) 
                        {
                                newcount[m].item=right[j].item;
                                newcount[m].count=right[j].count-1;
                                m++;
                        }
                        j++;
                }
        }
        else 
        if (i<l)
        { 
                while(i<l)
                {
                        newcount[m].item=left[i].item;
                        newcount[m].count=-1;
                        while ((newcount[m].item==left[i].item) && (i<l))
                        {
                                newcount[m].count+=left[i].count;
                                i++;
                        }
                        if (newcount[m].count>0) 
                                m++;
                        else 
                        { 
                                newcount[m].item=-1;
                                newcount[m].count=0;
                        }
                }
        }
  
        return(m);
}

void LC_Update(LC_type * lc, int val)
{
        Counter *tmp;

        // interpret a negative item identifier as a removal
        if (val>0) 
        { 
                lc->bucket[lc->buckets].item=val;    
                lc->bucket[lc->buckets].count=1;
        }
        else
        {
                lc->bucket[lc->buckets].item=-val;
                lc->bucket[lc->buckets].count=-1;
        }
  
        if (lc->buckets==lc->window) 
        {
                countershell(lc->window,lc->bucket);
                lc->holdersize=countermerge(lc->newcount,lc->bucket,lc->holder,
                                  lc->window,lc->holdersize,lc->maxholder);

                tmp=lc->newcount;
                lc->newcount=lc->holder;
                lc->holder=tmp;
                lc->buckets=0;
                lc->epoch++;
        }
        else 
                lc->buckets++;
}

int LC_Size(LC_type * lc, int num)
{
        int size;
        if ( num == -1 ) { // total
                size=(lc->maxholder+lc->window)*sizeof(Counter)+sizeof(LC_type);
        }
        else {
                size=(num)*sizeof(Counter)+sizeof(LC_type);
        }
        return size;
}

Counter *  LC_Output(LC_type * lc, int thresh)
{
        int i,point;  

        Counter * results;

        results=(Counter*) calloc(lc->window, sizeof(Counter));
        point=1;
        // should do a countermerge here.

        for (i=0;i<lc->holdersize;i++) 
        {
                if (lc->holder[i].count+lc->epoch >= thresh)
                results[point].item=lc->holder[i].item;
                results[point].count=lc->holder[i].count;
                point++;
        }
        results[0].item=point-1; // return number
        return results;
}

//show result and free memory
void lossy_show_result(Counter *result) {
        int len = 0;
        int i;

        len = result[0].item;
        printf("Item            Count\n");

        for (i = 1; i <= len; i++) {
                printf("%-16d%-16d\n", result[i].item, result[i].count);
        }
        /*
        for (i = len; i >= 0; i--) {
                free(&result[i]);
        }*/
}

//tcp sockets functions
int recv_key_tcp(char *server_port, char *lossy_phi)
{
        int sockfd, rc;
        int yes=1;
        struct addrinfo hints, *server;
        char message[256];
        float phi;
        LC_type *lcounter;
        int recv_count;
        Counter * result;

        phi = atof(lossy_phi);
        if (phi == 0) {
                perror("phi value error");
                exit(-1);
        }
        lcounter = LC_Init(phi);

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

        recv_count = 0;
        /* Loop forever accepting new connections. */
        while(1) {
                struct sockaddr_storage client_addr;
                socklen_t addr_size;
                int clientfd;
                int bytes_read;

                addr_size = sizeof client_addr;
                clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

                /* Use a loop to receive couters and reply them */
                while (1) {
                        bytes_read = read(clientfd, message, sizeof message);
                        if(bytes_read < 0) {
                                perror("ERROR reading socket");
                        }
                        if (bytes_read == 0) break;
                        if (bytes_read == 4) {
                                LC_Update(lcounter, *(int *)message);
                                recv_count++;
                                if (recv_count % 1000 == 0) {
                                        result = LC_Output(lcounter, 1);
                                        lossy_show_result(result);
                                }
                        }
                }

                close(clientfd);
        }

        freeaddrinfo(server);
        close(sockfd);
        LC_Delete(lcounter);
        return 0;
}

