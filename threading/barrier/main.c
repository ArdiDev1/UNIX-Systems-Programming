#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4

pthread_barrier_t barrier;
const char* messages[]  = {"Hello", "Whats Up", "Thats Cool", "Etc"};

typedef struct thread_arg_t {
    int id;
    const char* msg;
} thread_arg_t;

void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void* thread_main(void* arg) {
    thread_arg_t* thrd_arg = (thread_arg_t*) arg;

    //before wait
    printf("Thread %d is waiting..\n", thrd_arg->id);

    pthread_barrier_wait(&barrier);
    
    //after wait
    printf("Thread id = %d, msg: %s\n", thrd_arg->id, thrd_arg->msg);
    
    free(arg);
    return NULL;
}


int main(int argc, char** argv) {
    pthread_t thrds[NUM_THREADS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    for(int i = 0; i < NUM_THREADS; i++) {
        thread_arg_t* arg = malloc(sizeof(thread_arg_t));
        arg->id = i;
        arg->msg = messages[i];

        if(arg == NULL)
            perror("malloc"); 

        int rc = pthread_create(&thrds[i], NULL, thread_main, (void*) arg);
        if(rc > 0) {
            perror("pthread create");
        }
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_join(thrds[i], NULL);
        if(rc > 0) {
            perror("pthread join");
        }
    } 

    pthread_barrier_destroy(&barrier);
    return 0;
}