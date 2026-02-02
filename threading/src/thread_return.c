#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void* roll_dice() {
    int* value = (int*) malloc(sizeof(int));
    *value = (rand() % 6) + 1;
    return (void*) value;
}

int main() {
    int* res;
    srand(time(NULL));
    pthread_t th;
 
    if(pthread_create(&th, NULL, &roll_dice, NULL) != 0) {
        return 1;
    }

    // return argument is of void** 
    if(pthread_join(th, (void**) &res) != 0) {
        return 2;
    }

    printf("Result: %d\n", *res);
    free(res);
    return 0;
}