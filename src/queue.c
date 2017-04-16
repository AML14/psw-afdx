#include <unistd.h>

#include "queue.h"

/* IO pointers of the queue */
static int in_ptr = 0;
static int out_ptr = 0;

/* Fixed size circular queue */
static nqnode nodes[SIZE_QUEUE];

nqnode *next_nqnode_in()
{
    /* Mutex queue */
    int nptr;
    nqnode *node;

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
    /* End mutex */
    return (node);
}

nqnode *next_nqnode_out()
{
    nqnode *node;
    /* Lower semaphore count (initialized at SIZE_QUEUE)
     *                       (blocks until there is a node) */
    /* Mutex queue */
    if (out_ptr != in_ptr)
        node = &(nodes[out_ptr]);
    else
        /* This should never happen when using the semaphore */
        node = NULL;
    /* End mutex */
    return (node);
}

void dispose_last_nqnode()
{
    /* We cannot update pointer in next_nqnode_out() because the node might be
     * overwritten by an incoming packet before we are done with it */
    /* Mutex queue */
    if (out_ptr != in_ptr)
        out_ptr = (out_ptr +1) % SIZE_QUEUE;
    /* End mutex */
}
