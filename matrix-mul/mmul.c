#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

// Search TODO to find the locations where code needs to be completed

#define     NUM_THREADS     2

typedef struct {
    unsigned int id;
    TMatrix *m, *n, *t;
} thread_arg_t;

static void * thread_main(void * p_arg)
{
    thread_arg_t* matr_arg = (thread_arg_t*)p_arg;
    unsigned int offset = 0;

    if(matr_arg->id != 1)
        offset = matr_arg->m->nrows / 2;

    for (int i = 0; i < matr_arg->m->nrows / 2; i++)  {
        for (unsigned int j = 0; j < matr_arg->n->ncols; j++) {
            TElement sum = (TElement)0;
            for (unsigned int k = 0; k < matr_arg->m->ncols; k++)
                sum += matr_arg->m->data[i + offset][k] * matr_arg->n->data[k][j];
            matr_arg->t->data[i + offset][j] = sum;
        }
    }

    return NULL;
}

/* Return the sum of two matrices.
 *
 * If any pthread function fails, report error and exit. 
 * Return NULL if anything else is wrong.
 *
 * Similar to mulMatrix, but with multi-threading.
 */
TMatrix * mulMatrix_thread(TMatrix *m, TMatrix *n)
{
    if ( m == NULL || n == NULL
         || m->ncols != n->nrows )
        return NULL;

    TMatrix * t = newMatrix(m->nrows, n->ncols);
    if (t == NULL)
        return t;

    pthread_t thrd_id1, thrd_id2;
    thread_arg_t thread_args[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].m = m;
        thread_args[i].n = n;
        thread_args[i].t = t;
        thread_args[i].id = i + 1;
    }

    

    pthread_create(&thrd_id1, NULL, thread_main, (void*) &thread_args[0]);
    pthread_create(&thrd_id2, NULL, thread_main, (void*) &thread_args[1]);
    
    pthread_join(thrd_id1, NULL);
    pthread_join(thrd_id2, NULL);
    // TODO
    return t;
}
