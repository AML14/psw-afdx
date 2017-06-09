#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "simplog.h"

#include "scheduler.h"

typedef struct scheduler_node_s scheduler_node_t;
struct scheduler_node_s {
    timespec_t interval;       // How often to re-schedule
    timespec_t next_time;      // Next time to run
    void (*action)(void *);    // What to call
    void *data;                // Argument passed to action
    scheduler_node_t *prev;
    scheduler_node_t *next;
};

static scheduler_node_t *first = NULL;
static scheduler_node_t *last = NULL;

static pthread_mutex_t scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t wakeup_cond = PTHREAD_COND_INITIALIZER;
static pthread_t scheduler;

long start_scheduler();

void init_sched() {
    start_scheduler();
}

/* Timespec manipulation */

/* Check time a is greater or equal than time b */
#define tm_ge(a, b) ((a).tv_sec > (b).tv_sec ? 1 : \
            ((a).tv_sec < (b).tv_sec ? 0 : (a).tv_nsec >= (b).tv_nsec))

/* Increment time v with i */
#define tm_incr(v,i) do { \
        long nsecs = ((v).tv_nsec + (i).tv_nsec); \
        (v).tv_sec += (i).tv_sec + nsecs / 1000000000L; \
        (v).tv_nsec = nsecs % 1000000000L; } while(0)

/* These are the queue operations. The caller must lock before calling them. */

static void insert_node(scheduler_node_t *node)
{
    scheduler_node_t *next_node, *prev_node;
    timespec_t stime = node->next_time;
    prev_node = NULL;
    next_node = first;
    while(next_node) // Find node that has time sooner than me
    {
        if (tm_ge(stime, next_node->next_time)) // I go after this one, need to continue searching
        {
            prev_node = next_node;
            next_node = next_node->next;
        }
        else // I go before this one
        {
            node->prev = prev_node;
            node->next = next_node;
            next_node->prev = node;
            if(prev_node)
                prev_node->next = node;
            else
                first = node;
            break;
        }
    }
    if (!next_node) // We got to the end of the queue
    {
        node->prev = last;
        if (last) // We are the last one and possibly the only one
            last->next = node;
        last = node;
        node->next = NULL;
        if(!first) // We are the only one
            first = node;
    }
}

/* Perform corresponding operations to unchain a node and chain previous
 * with next. */
static void remove_node(scheduler_node_t *node)
{
    if (node->prev)
        node->prev->next = node->next;
    else
        first = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        last = node->prev;
}

/* Obtain the following (first) node */
static scheduler_node_t *pop()
{
    scheduler_node_t *node;
    node = first;
    if (first)
    {
       first = first->next;
       if (first)
           first->prev = NULL;
       else
           last = NULL;
    }
    return node;
}

/* Fills a node with passed data and inserts it in the queue */
void *schedule_ts(void (*action)(void *), void *data, timespec_t *interval)
{
    scheduler_node_t *node, *cur_first;
    timespec_t stime;
    node = (scheduler_node_t *)malloc(sizeof(scheduler_node_t));
    if (node == NULL)
    {
        simplog.writeLog(SIMPLOG_FATAL,"sched: error of memory");
        perror("sched: error of memory");
        exit(1);
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, (struct timespec *)&stime);
        node->action = action;
        node->data = data;
        node->interval = *interval;
        tm_incr(stime, *interval);
        node->next_time = stime;
        pthread_mutex_lock(&scheduler_mutex);
        cur_first = first;
        insert_node(node);
        if (first != cur_first)
            pthread_cond_signal(&wakeup_cond);
        pthread_mutex_unlock(&scheduler_mutex);
    }
    return node;
}

/* Calls schedule_ts but the parameters are passed in milliseconds */
void *schedule(void (*action)(void *), void *data, long interval)
{
    timespec_t i;
    i.tv_sec = interval / 1000;
    i.tv_nsec = (interval % 1000) * 1000000;
    return schedule_ts(action, data, &i);
}

/* Removes node from the schedule using the handler that schedule_ts returned */
void unschedule(void *handler)
{
    pthread_mutex_lock(&scheduler_mutex);
    remove_node((scheduler_node_t *)handler);
    pthread_mutex_unlock(&scheduler_mutex);
    free(handler);
}

void *scheduler_run(void *p)
{
    scheduler_node_t  *node, *qfirst;
    timespec_t now, next_time;
    int did_something;

    pthread_mutex_lock(&scheduler_mutex);
    while(1)
    {
        simplog.writeLog(SIMPLOG_DEBUG,"Scheduler thread");
        clock_gettime(CLOCK_REALTIME, (struct timespec *)&now);
        did_something = 0;
        /* While there is a first node and 'now' is later than the scheduler time
         * for the first node: */
        while((qfirst = first) && tm_ge(now, qfirst->next_time))
        {
            did_something = 1;
            node = pop(); // Before doing anything, we reschedule
            /* Increase next_time */
            tm_incr(node->next_time, node->interval);
            if tm_ge(now, node->next_time)
            {
                // Skip if missed
                node->next_time = now;
                tm_incr(node->next_time, node->interval);
            }
            insert_node(node);
            pthread_mutex_unlock(&scheduler_mutex);
            (*node->action)(node->data);
            pthread_mutex_lock(&scheduler_mutex);
        }
        if(!did_something)
        {
            // Wait for the next task to run
            if(qfirst)
            {
                // We get the time-to-wait from the first node
                next_time = qfirst->next_time;
                // Wake when time is over or condition triggers (schedule_ts)
                pthread_cond_timedwait(&wakeup_cond, &scheduler_mutex,
                        (struct timespec *)&(qfirst->next_time));
            }
            else
            {
                // Wait for the first thing to be scheduled
                pthread_cond_wait(&wakeup_cond, &scheduler_mutex);
            }
        }
    }
    return NULL;
}

long start_scheduler()
{
    return pthread_create(&scheduler, NULL, scheduler_run, NULL);
}
