#ifndef RING_H
#define RING_H

/* A ring buffer for multi-threaded programs. A single producer pushes
 * objects into the ring, while a single consumer pops them off for
 * processing. If the ring is full (i.e., the consumer is slower than the
 * producer), then the push operation fails.
*/

struct ring
{
	int size;
	int pop;
	int push;
	int count;
	void** items;
};


/* Create a ring buffer with the specified size. Return the ring or NULL
   if there is not enough memory to create. */
struct ring* ring_create(int size);

/* Add an entry to the ring.
        Return 0 on success, or a sensible error code if ring is full
*/
int ring_push(struct ring *rb, void* data);

/* Remove an entry from the ring.
        Return a pointer to the data, or NULL if empty
*/
void* ring_pop(struct ring *rb);

#endif
