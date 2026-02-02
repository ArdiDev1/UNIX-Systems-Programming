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

int get_pi(int n, unsigned int s) {
    int total = 0;
    unsigned int x, y;

    for(int i = 0; i < n; i++) {
        x = rand_double(&s);
        y = rand_double(&s);

        if(x*x + y*y <= 1)
            total++;
    }

    return total;
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

  total_count = get_pi(n, 0);


  double pi = 4.0 * total_count /(double)n;
  printf("pi = %.2f\n", pi);

  double end = now();
  printf("Time: %.6f seconds\n", end - start);

  return 0;
}
