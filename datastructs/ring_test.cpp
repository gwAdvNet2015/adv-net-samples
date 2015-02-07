

/* ring_test.cpp 
a test consol to see if ringbuffer's exceptions are working fine 
with exceeding push and pop options
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include "ring.h"

using namespace std;


void main()
{
	const int buffer_size = 5;
	const int push_size = 7;
	int pop_size = 12;


	void* data[buffer_size];
	void* pop_data;

	struct ring* ring_buffer = ring_create( buffer_size );



	for( int i = 0; i < push_size  ; i++){

		data[i] = new int[1];
		//data[i] = new char[1];

		int err = ring_push(ring_buffer, &data[i]);


		if ( err == 0 ){
			printf("push data %d = %d \n", i, &data[i]);
		}
		else{
			printf("push %d failed \n ", i);
		}
	}


	for( int i = 0; i < pop_size  ; i++){
		pop_data = ring_pop(ring_buffer);

		if(pop_data == NULL){
			printf("pop %d failed \n" , i);
		}
		else{
			printf("pop data %d = %d\n", i, pop_data);
		}
	}


	//	enter any word and hit enter to close the promp
	std::cout << "enter any word and hit enter to close the promp" << std::endl ;
	int end;
	std::cin >> end ; 


}
