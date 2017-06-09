#include <pthread.h>
#include "simplog.h"

#include "queue.h"
#include "globals.h"

/* IO pointers of the queue */
static int in_ptr = 0;
static int out_ptr = 0;

/* Fixed size circular queue */
static nqnode_t nodes[SIZE_QUEUE];
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER; // 1
static sem_t nodes_queue_sem;
sem_t *nr_nodes_queue = &nodes_queue_sem; // 0

nqnode_t *next_nqnode_in()
{
    int nptr;
    nqnode_t *node;

    simplog.writeLog(SIMPLOG_DEBUG,"In:%d, out:%d", in_ptr, out_ptr);
    /* Mutex queue */
    pthread_mutex_lock(&queue_mutex);

    nptr = (in_ptr + 1) % SIZE_QUEUE;
    simplog.writeLog(SIMPLOG_DEBUG,"Current ptr: %d", nptr);
    if (nptr == out_ptr)
    {
        /* Queue is full */
        node = NULL;
    }
    else
    {
        node = &nodes[in_ptr];
        in_ptr = nptr;
    }
    /* Raise semaphore count */
    sem_post(nr_nodes_queue);
    /* End mutex */
    pthread_mutex_unlock(&queue_mutex);
    return (node);
}

nqnode_t *next_nqnode_out()
{
    nqnode_t *node;

    /* Lower semaphore count (blocks until there is a node) */
    sem_wait(nr_nodes_queue);
    /* Mutex queue */
    pthread_mutex_lock(&queue_mutex);

    if (out_ptr != in_ptr)
        node = &(nodes[out_ptr]);
    else
        /* This should never happen when using the semaphore */
        node = NULL;
    /* End mutex */
    pthread_mutex_unlock(&queue_mutex);
    return (node);
}

void dispose_last_nqnode()
{
    /* We cannot update pointer in next_nqnode_out() because the node might be
     * overwritten by an incoming packet before we are done with it */

    /* Mutex queue */
    pthread_mutex_lock(&queue_mutex);

    if (out_ptr != in_ptr)
        out_ptr = (out_ptr +1) % SIZE_QUEUE;
    /* End mutex */
    pthread_mutex_unlock(&queue_mutex);
}

void init_queue()
{
	sem_init(nr_nodes_queue,0,0);
}
