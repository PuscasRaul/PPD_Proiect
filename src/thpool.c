#include "thpool.h"
#include <stdlib.h>
#include "logger.h"

/*
 * Initializes a counting semaphore
 * :param csem: the semaphore
 * :return: The initialized counting semaphore
 *          NULL if csem is NULL
 */
csem *init_csem(csem *csem) {
  if (csem == NULL)
    return NULL;

  int status = 0;

  pthread_mutex_init(&csem->mtx, NULL);
  status = pthread_cond_init(&csem->cond, NULL);
  if (status != 0) {
    pthread_mutex_destroy(&csem->mtx);
    return NULL;
  }

  csem->value = 0;
  return csem;
}

/*
 * Deinitializes the counting semaphore
 * :param csem: the semaphore
 * NOTE: safe to call with csem == NULL
 */
void deinit_csem(csem *csem) {
  if (csem) {
    pthread_mutex_destroy(&csem->mtx);
    pthread_cond_destroy(&csem->cond);
    csem->value = 0;
  }
}

/*
 * Blocks and waits until csem->value is > 0
 * param csem: the counting semaphore
 * 
 * The function acquires the mutex, unlocks it while waiting for the
 * csem->value to be positive. 
 * When a signal has been received, it will lock csem->mtx again,
 * decrement csem->value by 1, unlock the mutex and return.
 * 
 */
void csem_wait(csem *csem) {
  int status = 0;
  status = pthread_mutex_lock(&csem->mtx);
  if (status != 0) {
    return;
  }
  
  while (csem->value <= 0) {
    status = pthread_cond_wait(&csem->cond, &csem->mtx);
    if (status != 0) {
      pthread_mutex_unlock(&csem->mtx);
      return;
    }
  }

  csem->value--;
  pthread_mutex_unlock(&csem->mtx);
}

/*
 * Signals that the value of csem->value has changed
 * :param csem: the counting semaphore
 *
 * Acquires the mutex, increments csem->value, signals change and
 * unlocks the mutex.
 */
void csem_signal(csem *csem) {
  int status = 0;
  status = pthread_mutex_lock(&csem->mtx);
  if (status != 0) {
    return;
  }
  csem->value++;

  status = pthread_cond_signal(&csem->cond);
  if (status != 0) {
    pthread_mutex_unlock(&csem->mtx);
    return;
  }

  pthread_mutex_unlock(&csem->mtx);
}

job *init_job(job *job, void (*func)(void *args), void *args) {
  if (job == NULL)
    return NULL;

  job->func = func;
  job->args = args; 
  return job;
}

void deinit_job(job *job) {
  if (job != NULL) {
    job->next = NULL;
    job->args = NULL;
    job->func = NULL;
  }
}

static inline job *new_job(void (*func)(void *args), void *args) {
  return init_job(malloc(sizeof(job)), func, args);
}

static inline void free_job(job *job) {
  if (job) {
    deinit_job(job);
    free(job);
  }
}

/*
 * Initializes the job queue
 * param: jb_queue: The job queue 
 * return: The address of the initialized job queue
 *         NULL if the initializes fails, or jb_queue was NULL 
 * WARN: Must be deinitialized through a call to: deinit_jb_queue
 */
job_queue *init_jb_queue(job_queue *jb_queue) {
  if (jb_queue == NULL)
    return NULL;

  init_csem(&jb_queue->semaphore);
  pthread_mutex_init(&jb_queue->rw_mtx, NULL);
  jb_queue->front = jb_queue->rear = NULL;
  return jb_queue;
}

/*
 * Deinitializes job queue
 * param: jb_queue: queue to be deinitialized
 * NOTE: Safe to call for NULL pointers
 */
void deinit_jb_queue(job_queue *jb_queue) {
  if (jb_queue != NULL) {
    deinit_csem(&jb_queue->semaphore);
    pthread_mutex_destroy(&jb_queue->rw_mtx);

    job *prev = NULL;
    while (jb_queue->front != NULL) {
      prev = jb_queue->front;
      jb_queue->front = jb_queue->front->next;
      free_job(prev);
    }
  }
}

/*
 * Pushes an element into the job_queue
 * param: jb_queue: queue in which we push
 * param: jb: the job we push
 * side_effect: Will modify the condition variable has_jobs to true
 * NOTE: Safe to call with NULL queue/ job
 */
void jb_queue_push(job_queue *jb_queue, job *jb) {
  if (jb_queue == NULL)
    return;

  pthread_mutex_lock(&jb_queue->rw_mtx);

  if (jb_queue->front == NULL) 
    jb_queue->front = jb_queue->rear = jb;
  else {
    jb_queue->rear->next = jb;
    jb_queue->rear = jb;
  }

  pthread_mutex_unlock(&jb_queue->rw_mtx);
  csem_signal(&jb_queue->semaphore);
}

/* 
 * Pops an element from the job_queue
 * param: jb_queue: the queue from which we pop
 * return: First element added to the queue, if there is any
 *         NULL if jb_queue is empty, or if jb_queue is NULL
 */
job *jb_queue_pop(job_queue *jb_queue) {
  if (jb_queue == NULL)
    return NULL;

  /* Wait untill there are elements in the queue */
  csem_wait(&jb_queue->semaphore);
  pthread_mutex_lock(&jb_queue->rw_mtx);
  if (jb_queue->front == NULL) {
    pthread_mutex_unlock(&jb_queue->rw_mtx);
    return NULL;
  }

  job *jb = jb_queue->front;
  jb_queue->front = jb_queue->front->next;

  if (jb_queue->front == NULL)
    jb_queue->rear = NULL;

  pthread_mutex_unlock(&jb_queue->rw_mtx);
  return jb;
}

thpool *new_thpool(int thrd_count) {
  return init_thpool(malloc(sizeof(thpool)), thrd_count);
}

void free_thpool(thpool *thpool) {
  if (thpool) {
    deinit_thpool(thpool);
    free(thpool);
  }
}

thpool *init_thpool(thpool *thpool, int thrd_count) {
  if (thpool == NULL)
    return NULL;

  if (thrd_count < 1)
    return NULL;

  if (init_jb_queue(&thpool->jobs) == NULL) {
    return NULL;
  }

  thpool->threads_count = thrd_count;
  thpool->threads = malloc(thrd_count * sizeof(pthread_t));
  if (thpool->threads == NULL) {
    deinit_jb_queue(&thpool->jobs);
    return NULL;
  }

  log_info("thpool initialized with: [%d] threads", thrd_count);

  return thpool;
}

void deinit_thpool(thpool *thpool) {
  if (thpool != NULL) {
    deinit_jb_queue(&thpool->jobs);
    if (thpool->threads) free(thpool->threads);

    *thpool = (struct thpool) {};
  }
}

static void thr_func(void *args) {
  job_queue *jb_queue = (job_queue*) args; 

  while (1) {
    job *jb = jb_queue_pop(jb_queue); 
    /* Handle poison pill */
    if (jb->func == NULL)
      break;

    if (jb->func)
      jb->func(jb->args); 

    free_job(jb);
  }
}

void thpool_run(thpool *thpool) {
  if (thpool == NULL || thpool->threads == NULL)
    return;

  log_info("Thread pool is currently running");
  for (register int i = 0; i < thpool->threads_count; i++)
    pthread_create(
        &thpool->threads[i],
        NULL,
        (void *)thr_func,
        &thpool->jobs
        );
}

void thpool_poison_pill(thpool *thpool) {
  for (int i = 0; i < thpool->threads_count; i++)
    thpool_addwork(thpool, NULL, NULL);
}

void thpool_join(thpool *thpool) {
  if (thpool == NULL || thpool->threads == NULL)
    return;

  for (register int i = 0; i < thpool->threads_count; i++) 
    pthread_join(thpool->threads[i], NULL);
}

void thpool_addwork(
    thpool *thpool,
    void (*func)(void *args),
    void *args
    ) {
  if (thpool == NULL)
    return;
  
  job *jb = new_job(func, args);
  if (jb == NULL) {
    log_error("Failed creating a new job");
    return;
  }

  jb_queue_push(&thpool->jobs, jb);
  log_info("Successfully pushed a new job");
}
