// ring.c : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>

#include "ring.h"




/* Create a ring buffer with the specified size. Return the ring or NULL
if there is not enough memory to create. */



struct ring* ring_create(int size)
{
	struct ring *rb = (struct ring*)malloc(sizeof(struct ring ));

	if (rb == NULL) {
		printf("Does not have enough memory for creating ring_buffer.\n");
		return NULL;
	}

	rb->size = size;
	rb->pop = 0;
	rb->push = 0;
	rb->count = 0;

	rb->items = (void**)malloc(sizeof(void*) );

	if (rb->items == NULL) {
		printf("Does not have enough memory for creating item.\n");
		return NULL;
	}

	return rb;
};



/* Add an entry to the ring.
Return 0 on success, or a sensible error code if ring is full
*/
int ring_push(struct ring *rb, void* data)
{
	if ( rb->count < rb->size ){
		rb->items[ rb->push ] = data;
		rb->push = (rb->push + 1) % rb->size;
		rb->count = rb->count +1;

		return 0;
	}
	else{
		return -1;
	}

}



/* Remove an entry from the ring.
Return a pointer to the data, or NULL if empty
*/
void* ring_pop(struct ring *rb)
{
	if ( rb->count <= 0 ){
		return NULL;
	}
	else{   
		void* temp;
		temp = rb->items[ rb->pop ];
		rb->pop = (rb->pop + 1) % rb->size;
		rb->count = rb->count - 1; 

		return  temp;
	}
}

