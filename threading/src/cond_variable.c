#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t fuel_mutex;
pthread_cond_t cond_fuel;

int fuel = 0;

//produces fuel. 
// mutex is unlocked when cond_wait is called,

void* fill_fuel(void* arg) {
    for(int i = 0; i < 5; i++) {
        pthread_mutex_lock(&fuel_mutex);
        fuel += 15;
        printf("Filling fuel... %d\n", fuel);
        pthread_mutex_unlock(&fuel_mutex);
        pthread_cond_signal(&cond_fuel);

        sleep(1);
    }

}

//takes fuel.
void* car(void* arg) {
    pthread_mutex_lock(&fuel_mutex);

    while(fuel < 40) {
        printf("No fuel. Waiting...\n");
        pthread_cond_wait(&cond_fuel, &fuel_mutex);
        // Equivalent to:
        // pthread_mutex_unlock()
        // wait for signal 
        // pthread mutex lock 
    }

    fuel -= 40;

    printf("Got fuel.. left %d\n", fuel);
    pthread_mutex_unlock(&fuel_mutex);

}

int main(int argc, char** argv) { 
    pthread_t thrd[2];
    pthread_mutex_init(&fuel_mutex, NULL);
    pthread_cond_init(&cond_fuel, NULL);

    for(int i = 0; i < 2; i++) {
        if(i == 1) {
            pthread_create(&thrd[i], NULL, &car, NULL);
        }

        pthread_create(&thrd[i], NULL, &fill_fuel, NULL);
    }

    for(int i = 0; i < 2; i++) {
        pthread_join(thrd[i], NULL);
    }

    pthread_mutex_destroy(&fuel_mutex);
    pthread_cond_destroy(&cond_fuel);
    return 0;
}