#include <stdlib.h>
#include <stdio.h>
#include "simplog.h"
#include "application_manager.h"
#include "scheduler.h"
#include "globals.h"

reg_app_t *registered_applications;
static sem_t registering_mutex_val;
sem_t *registering_mutex = &registering_mutex_val; // 1

static void serve_scheduler_query(void *data);

void register_query(int fd, app_query_t *query, long interval)
{
    reg_app_t *app;
    reg_query_node_t *node;
    void *handler;

    /* Mutex registering */
    sem_wait(registering_mutex);

    app = registered_applications;

    while (app && app->fd != fd)
        app = app->next;
    if (app == NULL)
    {
        /* We have to register a new application */
        app = (reg_app_t *)malloc(sizeof(reg_app_t));
        if (app == NULL)
        {
            /* End mutex registering */
            sem_post(registering_mutex);
            /* No memory */
            simplog.writeLog(SIMPLOG_FATAL,"Error of memory");
            perror("Error of memory");
            exit(1);
        }
        app->fd = fd;
        app->queries = NULL;
        app->next = registered_applications;
        registered_applications = app;
    }
    node = (reg_query_node_t *)malloc(sizeof(reg_query_node_t));
    if (node == NULL)
    {
        /* End mutex registering */
        sem_post(registering_mutex);
        /* No memory */
        simplog.writeLog(SIMPLOG_FATAL,"Error of memory");
        perror("Error of memory");
        exit(1);
    }
    simplog.writeLog(SIMPLOG_DEBUG,"Inside register_query: %d", query->data_id);
    node->query = *query;
    node->next = app->queries;
    node->app = app;
    handler = schedule(serve_scheduler_query, node, interval);
    node->handler = handler;
    app->queries = node;
    /* End mutex registering */
    sem_post(registering_mutex);
}

void unregister_query(int fd, uint32_t req_id)
{
    reg_app_t *app;
    reg_query_node_t *node, *prev;

    simplog.writeLog(SIMPLOG_DEBUG,"Start unregister_query");
    /* Mutex registering */
    sem_wait(registering_mutex);

    app = registered_applications;

    while (app && app->fd != fd)
        app = app->next;
    if (app == NULL)
    {
        /* End mutex registering */
        sem_post(registering_mutex);
        return;
    }
    node = app->queries;
    prev = NULL;
    while(node)
    {
        if(node->query.req_id == req_id)
        {
            if(prev)
            {
                /* Unchain the node */
                prev->next = node->next;
            }
            else
            {
                /* Was the first */
                app->queries = node->next;
            }
            unschedule(node->handler);
            free((void *)node);
            break;
        }
        else
        {
            prev = node;
            node = node->next;
        }

    }
    /* End mutex registering */
    sem_post(registering_mutex);
    simplog.writeLog(SIMPLOG_DEBUG,"Query desregistered");
}

void unregister_application(int fd)
{
    reg_app_t *app, *prev;
    reg_query_node_t *node, *next;

    simplog.writeLog(SIMPLOG_DEBUG,"Start unregister_application");
    /* Mutex registering */
    sem_wait(registering_mutex);

    app = registered_applications;
    prev = NULL;
    while (app)
    {
        if(app->fd == fd)
        {
            if(prev)
            {
                /* Unchain the app */
                prev->next = app->next;
            }
            else
            {
                /* Was the first */
                registered_applications = app->next;
            }
            break;
        }
        prev = app;
        app = app->next;
    }
    if (app == NULL)
    {
        /* End mutex registering */
        sem_post(registering_mutex);
        return;
    }
    node = app->queries;
    simplog.writeLog(SIMPLOG_DEBUG,"unregister_app: before going over queries");
    while (node)
    {
        unschedule(node->handler);
        next = node->next;
        free((void *)node);
        node = next;
    }
    simplog.writeLog(SIMPLOG_DEBUG,"unregister_app: after going over queries");
    free((void *)app);
    /* End mutex registering */
    sem_post(registering_mutex);
    simplog.writeLog(SIMPLOG_DEBUG,"App unregistered");
}

/* This is the function passed to the scheduler */
static void serve_scheduler_query(void *data)
{
    reg_query_node_t *node = (reg_query_node_t *)data;
    int fd = node->app->fd;

    serve_query(fd, &(node->query));
}
