#include <unistd.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

void err_exit(const char* msg) {
    perror(msg);
    exit(-1);
}

//thread function
void* funct() {
    // shows that threads are running concurrently 
    printf("Hi from thread\n");
    // sleep between statements
    sleep(2);

    printf("Can you see this?\n");
    pthread_exit(NULL);
}


int main() {
    // id for thread 1
    pthread_t one, two; 
    int ret;
    
    // create the threads
    ret = pthread_create(&one, NULL, funct, NULL);
    if(ret > 0) 
        err_exit("pthread_create");

    ret = pthread_create(&two, NULL, funct, NULL);
    if(ret > 0) 
        err_exit("pthread_create");

    // waits for theads, with id to terminate
    ret = pthread_join(one, NULL);
    if(ret > 0)
        err_exit("pthread_join");

    ret = pthread_join(two, NULL);
    if(ret > 0)
        err_exit("pthread_join");

    return 0;
}