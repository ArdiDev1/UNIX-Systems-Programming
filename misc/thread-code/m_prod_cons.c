#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

/*
* declarations 
*/
#define MAX_BUF_SZ 100
static int buffer=0;

uint8_t n_consumer;
uint8_t n_producer;

typedef struct {
    int done;
    pthread_mutex_t mutex;
    pthread_cond_t sig_cons;
    pthread_cond_t sig_prod;
} sync_t;

typedef struct {
    int id;
    sync_t* sdata;
} thread_args;

/*
 * utility helpers 
*/
void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void init_sync_obj(sync_t* sync_ob) {
    sync_ob->done = n_consumer;
    pthread_mutex_init(&sync_ob->mutex, NULL);
    pthread_cond_init(&sync_ob->sig_cons, NULL);
    pthread_cond_init(&sync_ob->sig_prod, NULL);
}

void destroy_sync_obj(sync_t* sync_ob) {
    pthread_mutex_destroy(&sync_ob->mutex);
    pthread_cond_destroy(&sync_ob->sig_prod);
    pthread_cond_destroy(&sync_ob->sig_cons);
}

/*
   threaded function(s) 
*/
void* producer(void* arg) {
    thread_args* thrd_arg = (thread_args*) arg;
    sync_t* this_syncp = thrd_arg->sdata;

    while(this_syncp->done) {
        pthread_mutex_lock(&this_syncp->mutex);

        while (buffer >= MAX_BUF_SZ && this_syncp->done) {
            printf("\n-- Buffer is full, waiting --\n");
            pthread_cond_wait(&this_syncp->sig_prod, &this_syncp->mutex);
        }

        buffer += 5;
        printf("(P) Thread %d: +5, buffer = %d\n", thrd_arg->id, buffer);
        
        pthread_cond_broadcast(&this_syncp->sig_cons);
        pthread_mutex_unlock(&this_syncp->mutex);
    }

    return NULL;
}

void* consumer(void* arg) {
    thread_args* thrd_arg = (thread_args*) arg;
    sync_t* this_syncp = thrd_arg->sdata;

    pthread_mutex_lock(&this_syncp->mutex);

    while(buffer < 15) {
        printf("\n-- No items, waiting --\n");
        pthread_cond_wait(&this_syncp->sig_cons, &this_syncp->mutex);
    }

    buffer -= 15;
    this_syncp->done--;

    printf("(C) Thread %d: -15, buffer = %d\n", thrd_arg->id, buffer);
    
    pthread_cond_broadcast(&this_syncp->sig_prod);
    pthread_mutex_unlock(&this_syncp->mutex);

    return NULL;
}

/*
 * entrypoint  
*/
int main(int argc, char** argv) {
    if(argc < 3) {
        printf("usage: ./main <cosumer> <producer>\n");
        return -1;
    }

    n_consumer = atoi(argv[1]);
    n_producer = atoi(argv[2]);

    assert(n_consumer > 0 && n_consumer < INT8_MAX);
    assert(n_producer > 0 && n_producer < INT8_MAX);

    sync_t sync_ob;
    init_sync_obj(&sync_ob);

    pthread_t thrd_id[n_producer + n_consumer];
    thread_args thrd_args[n_producer + n_consumer];

    for (int i = 0; i < n_producer; i++) {
        thrd_args[i].id = i;
        thrd_args[i].sdata = &sync_ob;

        if(pthread_create(&thrd_id[i], NULL, producer, (void*)&thrd_args[i]) != 0) {
            die("creating producer threads");
        }
    }

    for (int i = n_producer; i < n_producer + n_consumer; i++) {
        thrd_args[i].id = i;
        thrd_args[i].sdata = &sync_ob;

        if(pthread_create(&thrd_id[i], NULL, consumer, (void*)&thrd_args[i]) != 0) {
            die("creating consumer threads");
        }
    }

    for (int i = 0; i < n_producer + n_consumer; i++) {
        if(pthread_join(thrd_id[i], NULL) != 0) {
            die("joining producer threads");
        }
    }

    destroy_sync_obj(&sync_ob);
    printf("\nMaster thread done..\n");
    return 0;
}