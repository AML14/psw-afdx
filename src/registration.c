#include <stdlib.h>
#include "application_manager.h"
#include "globals.h"

reg_app_t *registered_applications;

void register_query(int fd, app_query_t *query, long interval)
{
    reg_app_t *app;
    reg_query_node_t *node;

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
            /* Manage error */
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
        /* Manage error */
    }
    node->query = *query;
    node->next = app->queries;
    app->queries = node;
    /* End mutex registering */
    sem_post(registering_mutex);
}

void unregister_query(int fd, uint32_t req_id)
{
    reg_app_t *app;
    reg_query_node_t *node, *prev;

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
}

void unregister_application(int fd)
{
    reg_app_t *app, *prev;
    reg_query_node_t *node, *next;

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
    while (node)
    {
        next = node->next;
        free((void *)node);
    }
    free((void *)app);
    /* End mutex registering */
    sem_post(registering_mutex);
}
