#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

#define     NUM_THREADS     2

typedef struct {
    unsigned int start, end;
    TMatrix *m, *n, *t;
} thread_arg_t;

/* the main function of threads */
static void* thread_main(void* arg)
{
    // cast arg to correct type
    thread_arg_t* matr_arg = (thread_arg_t*) arg;
    unsigned int start, end, n_col; 

    n_col = matr_arg->m->ncols;

    start = matr_arg->start;
    end =  matr_arg->end;

    for (unsigned i = start; i < end; i++)
        for (unsigned int j = 0; j < n_col; j++)
            matr_arg->t->data[i][j] = matr_arg->m->data[i][j] + matr_arg->n->data[i][j];

    return NULL;
}


/* Return the sum of two matrices. The result is in a newly creaed matrix. 
 *
 * If a pthread function fails, report error and exit. 
 * Return NULL if something else is wrong.
 *
 * Similar to addMatrix, but this function uses 2 threads.
 */
TMatrix * addMatrix_thread(TMatrix *m, TMatrix *n)
{
    if (    m == NULL || n == NULL
         || m->nrows != n->nrows || m->ncols != n->ncols )
        return NULL;

    TMatrix *t = newMatrix(m->nrows, m->ncols);
    if (t == NULL)
        return t;

    thread_arg_t args[2];    
    args[0].t = args[1].t = t;
    args[0].m = args[1].m = m;
    args[0].n = args[1].n = n;

    args[0].start = 0;
    args[0].end = m->nrows / 2;

    args[1].start = m->nrows / 2;
    args[1].end = m->nrows;

    pthread_t thrd_1, thrd_2;

    pthread_create(&thrd_1, NULL, thread_main, (void*) &args[0]);
    pthread_create(&thrd_2, NULL, thread_main, (void*) &args[1]);
    
    pthread_join(thrd_1, NULL);
    pthread_join(thrd_2, NULL);

    // TODO
    return t;
}
