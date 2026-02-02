#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 10

int primes[10] = { 2 , 3, 5, 7 , 11, 13, 17, 19, 23, 29 };

void* thread_main(void* args) {
    int value = *(int*) args;
    printf("%d ", value);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    pthread_t thrds[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thrds[i], NULL, thread_main, (void*) (primes + i));
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thrds[i], NULL);
    }
    
    return 0;
}