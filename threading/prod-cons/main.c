#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2 
#define MAX 20
int buffer=0;

pthread_mutex_t mutex;

pthread_cond_t prod_cond;
pthread_cond_t cons_cond;

void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void* producer(void* args) {
 
    for(int i = 0; i < 100; i++) {
        pthread_mutex_lock(&mutex);
        
        while(buffer == MAX) {
            pthread_cond_wait(&prod_cond, &mutex);
            printf("Buffer is full..\n");
        }

        buffer += 5;
        printf("Added 5 to buffer\n");

        pthread_cond_signal(&cons_cond);
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }

    return NULL;
}

void* consumer(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        
        while(buffer < 5) {
            pthread_cond_wait(&cons_cond, &mutex);
            printf("Waiting for more..\n");
        }

        buffer -= 5;
        printf("Remove 5 from buffer\n");

        pthread_cond_signal(&prod_cond);
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }

    return NULL;
}

int main(int argc, char** argv) {
    pthread_t prod_thrd, cons_thrd;

    pthread_mutex_init(&mutex, NULL);

    pthread_cond_init(&cons_cond, NULL);
    pthread_cond_init(&prod_cond, NULL);

    pthread_create(&prod_thrd, NULL, producer, NULL);
    pthread_create(&cons_thrd, NULL, consumer, NULL);

    pthread_join(prod_thrd, NULL);
    pthread_cancel(cons_thrd);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&prod_cond);
    pthread_cond_destroy(&cons_cond);
    return 0;
}