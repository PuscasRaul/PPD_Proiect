#ifndef THPOOL_H
#define THPOOL_H

#include <pthread.h>
typedef struct job job;
typedef struct job_queue job_queue;
typedef struct csem csem;
typedef struct thpool thpool;

struct csem {
  pthread_mutex_t mtx;
  pthread_cond_t cond;
  int value;
};

struct job {
  void (*func)(void *args); /* Job function pointer */
  void *args; /* Arguments for the function */
  job *next; /* pointer to next job */
};

struct job_queue {
  csem semaphore;
  pthread_mutex_t rw_mtx;
  job *front; /* Pointer to front of queue */
  job *rear;  /* Pointer to end of queue */
};

struct thpool {
  job_queue jobs;
  pthread_t *threads; /* Thread array */
  int threads_count; /* thread array size */
};

/*
 * Initializes the thread pool.
 * param: thpool: the thread pool
 * param: thrd_count: the amount of threads in th_pool
 * return: The initialized thread pool on success.
 *         NULL on failure/ if thpool is NULL
 * WARN: object must be destroyed via call to deinit_thpool
 */
thpool *init_thpool(thpool *thpool, int thrd_count);

/*
 * Deinitializes the thread pool.
 * param: thpool: the thread pool
 * NOTE: Can be called with a NULL ptr
 */
void deinit_thpool(thpool *thpool);

/*
 * Allocated on the heap and initializes the thread pool.
 * param: thrd_count: the amount of threads in th_pool
 * return: The initialized thread pool on success.
 *         NULL on failure
 * WARN: object must be destroyed via call to free_thpool
 */
thpool *new_thpool(int thrd_count);

/*
 * Deinitializes and frees the thread pool.
 * param: thpool: the thread pool
 * NOTE: Can be called with a NULL ptr
 */
void free_thpool(thpool *thpool); 

/*
 * Creates and starts all the threads.
 * The threads will remain alive, and polling untill the user calls
 * thpool_poison_pill followed by thpool_join.
 *
 * On each loop, the threads poll for jobs. The jobs will be taken in
 * a FIFO mode.
 *
 * param: thpool: the thread pool
 */
void thpool_run(thpool *thpool);

/*
 * Joins all the threads owned by the thread pool.
 * param: thpool: the thread pool
 */
void thpool_join(thpool *thpool); 

/*
 * Adds work to the internal queue that the thpool holds.
 * If the function is called with func == NULL and args == NULL, that's
 * considered a poison pill and it will stop the executing thread that
 * consumes it.
 *
 * param: thpool: the thread pool
 * param: func: the function to be executed
 * param: args: function arguments
 */
void thpool_addwork(
    thpool *thpool,
    void (*func)(void *args),
    void *args
    ); 

/*
 * Sends a poison pill to all of the threads owned by the thread pool.
 * param: thpool: the thread pool
 */
void thpool_poison_pill(thpool *thpool); 
#endif // THPOOL_H
