#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define N_THREADS 2
#define SAMPLE_SIZE 10000000

double now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}


typedef struct thread_arg_tag {
  int id; //id for thread, used as seed for rand gen.
  int count; // n points with dist t0 (0,0) within 1
  int total; // number of points thread shoud generate
} thread_arg_t;

double rand_double(unsigned int *s) { return (rand_r(s) / (double)RAND_MAX); }

void *thread_main(void *thread_arg) {
  // cast back to right type
  thread_arg_t* thrd_arg = (thread_arg_t*) thread_arg;

  double x, y;
  unsigned int s = thrd_arg->id;
  int total_c = 0;

  int total = thrd_arg->total;
  for (int i = 0; i < total; i++) {
    x = rand_double(&s);
    y = rand_double(&s);

    if(x*x + y*y <= 1)
      total_c++;
  }
  
  thrd_arg->count = total_c;
  return NULL;
}

int main(int argc, char *argv[]) {

  int n = SAMPLE_SIZE;
  int n_threads = N_THREADS;
  int total_count = 0;

  if (argc > 1) {
    n = atoi(argv[1]);
  }
  if (argc > 2) {
    n_threads = atoi(argv[2]);
  }

  double start = now();

  pthread_t thread_arr[n_threads];
  thread_arg_t thread_args[n_threads];

  int points_gen = n / n_threads;

  // spawn threads;
  for (int i = 0; i < n_threads; i++) {
    thread_args[i].count = 0;
    thread_args[i].id = i;
    thread_args[i].total = points_gen;

    pthread_create(&thread_arr[i], NULL, thread_main, (void*)&thread_args[i]);
  }

  for(int i = 0; i <n_threads; i++) {
    pthread_join(thread_arr[i], NULL);
    total_count += thread_args[i].count;
  }

  double pi = 4.0 * total_count /(double)n;
  printf("pi = %.2f\n", pi);

  double end = now();
  printf("Time: %.6f seconds\n", end - start);

  return 0;
}
