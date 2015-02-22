/* ring_test.c
a test consol to see if ringbuffer's exceptions are working fine
with exceeding push and pop options
*/
#include <stdio.h>
#include <stdlib.h>



#include "../ring.h"

int
main()
{
	//set buffer size as 5, first push 3 elements, and then pop 2 elements, then push 6 elements, at last pop 12 elements
	const int buffer_size = 5;

	int push_size_1 = 3;
	int pop_size_1 = 2;
	int push_size_2 = 6;
	int pop_size_2 = 12;

	void* data[buffer_size];
	void* pop_data;

	struct ring* ring_buffer = ring_create( buffer_size );

	int i;

	for( i = 0; i < push_size_1  ; i++){

		data[i] = malloc(sizeof(int));

		int err = ring_push(ring_buffer, &data[i]);

		if ( err == 0 ){
			printf("push data %d = %d \n", i, &data[i]);
		}
		else{
			printf("push %d failed \n ", i);
		}
	}

	for( i = 0; i < pop_size_1  ; i++){
		pop_data = ring_pop(ring_buffer);

		if(pop_data == NULL){
			printf("pop %d failed \n" , i);
		}
		else{
			printf("pop data %d = %d\n", i, pop_data);
		}
	}

	for( i = 0; i < push_size_2  ; i++){

		data[i] = malloc(sizeof(int));

		int err = ring_push(ring_buffer, &data[i]);

		if ( err == 0 ){
			printf("push data %d = %d \n", i, &data[i]);
		}
		else{
			printf("push %d failed \n ", i);
		}
	}

	for( i = 0; i < pop_size_2  ; i++){
		pop_data = ring_pop(ring_buffer);

		if(pop_data == NULL){
			printf("pop %d failed \n" , i);
		}
		else{
			printf("pop data %d = %d\n", i, pop_data);
		}
	}

	return 0;
}
