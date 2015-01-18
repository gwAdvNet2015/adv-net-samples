#include<pthread.h>
#include<stdio.h>
#include<ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

/****************************************
        Author: Pradeep Kumar	 
        The file shows a demo on using threads.

        
****************************************/
#define DEFAULT_THREAD_COUNT 1

typedef struct __thread_param_t {
    pthread_t thread_id;
} thread_param_t;

/*
 * Taken from man pages example.
 */
#define handle_error_en(en, msg) \
                       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


/*
 * Thread Entry function.
 */
void* print_hello(void* arg)
{
        thread_param_t* thread_param = (thread_param_t*)(arg);
        printf("Hello from thread Id %d\n", thread_param->thread_id);
        return (void*)0;
}

int main(int argc, char ** argv) 
{
        int             error_code = 0;
        void*           status = 0;
        size_t          stack_size = 2*1024*1024; /* 2MB*/
        int             thread_count = DEFAULT_THREAD_COUNT;
        pthread_attr_t  thread_attr;

        /*
         * Ideally, this objects should not get destroyed before the
         * worker thread. It is safe to define here as it is main thrad's stack.
         */
        thread_param_t  thread_param;
        
        /* 
         * Initialize the thread attribute 
         */
        error_code = pthread_attr_init(&thread_attr);
        if ( 0 != error_code) {
                handle_error_en(error_code, "pthread_attr_init");
        }
         
        /* 
         * Let's do some custom configuration, like
         * a different stack size.
         */
        error_code = pthread_attr_setstacksize(&thread_attr, stack_size);
        if ( 0 != error_code) {
                handle_error_en(error_code, "pthread_attr_setstacksize");
        }

        /*
         * Create the thread. 
         */
        error_code = pthread_create(&thread_param.thread_id,
                                    &thread_attr,
                                    print_hello,
                                    &thread_param);
        if ( 0 != error_code) {
                handle_error_en(error_code, "pthread_create");
        }
        
        stack_size = 0;
        error_code = pthread_attr_getstacksize(&thread_attr, &stack_size);
        if ( 0 != error_code) {
                handle_error_en(error_code, "pthread_attr_getstacksize");
        }

        /*destroy the thread attribute object, as it is no longer needed. */
        error_code = pthread_attr_destroy(&thread_attr);
        if (0 != error_code) {
                handle_error_en(error_code, "pthread_attr_destroy");
        }
        
        /*
         * Wait for thread to finish executing.
         */
        error_code = pthread_join(thread_param.thread_id, &status);
        if (0 != error_code) {
                handle_error_en(error_code, "pthread_join");
        }
        
        /*
         * Printing the stack size of the worker thread.
         */
        printf("Thread's stack size is: %d Bytes\n", stack_size);

        return 0;
}
