#include "queue.h"
#include "globals.h"

/* IO pointers of the queue */
static int in_ptr = 0;
static int out_ptr = 0;

/* Fixed size circular queue */
static nqnode nodes[SIZE_QUEUE];

nqnode *next_nqnode_in()
{
    int nptr;
    nqnode *node;

    /* Mutex queue */
    sem_wait(queue_mutex);

    nptr = (in_ptr + 1) % SIZE_QUEUE;
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
    sem_post(queue_mutex);
    return (node);
}

nqnode *next_nqnode_out()
{
    nqnode *node;

    /* Lower semaphore count (blocks until there is a node) */
    sem_wait(nr_nodes_queue);
    /* Mutex queue */
    sem_wait(queue_mutex);

    if (out_ptr != in_ptr)
        node = &(nodes[out_ptr]);
    else
        /* This should never happen when using the semaphore */
        node = NULL;
    /* End mutex */
    sem_post(queue_mutex);
    return (node);
}

void dispose_last_nqnode()
{
    /* We cannot update pointer in next_nqnode_out() because the node might be
     * overwritten by an incoming packet before we are done with it */

    /* Mutex queue */
    sem_wait(queue_mutex);

    if (out_ptr != in_ptr)
        out_ptr = (out_ptr +1) % SIZE_QUEUE;
    /* End mutex */
    sem_post(queue_mutex);
}
