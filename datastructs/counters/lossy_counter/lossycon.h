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

/* for lossy counting */
typedef struct counter
{
        int item;
        int count;
} Counter;

typedef struct LC_type
{
        Counter *bucket;
        Counter *holder;
        Counter *newcount;
        int buckets;
        int holdersize;
        int maxholder;
        int window;
        int epoch;
} LC_type;

// conter functions
LC_type * LC_Init(float);
void LC_Delete(LC_type *);
void LC_Update(LC_type *, int);
int LC_Size(LC_type *, int);
Counter * LC_Output(LC_type *,int);
