#ifndef APPLICATION_MANAGER_H_
#define APPLICATION_MANAGER_H_

#include "application_protocol.h"

typedef struct reg_app_s reg_app_t;
typedef struct reg_query_node_s reg_query_node_t;

struct reg_query_node_s {
    app_query_t query;
    reg_query_node_t *next;
};

struct reg_app_s {
    int fd;
    reg_query_node_t *queries;
    reg_app_t *next;
};

/* Registers an application for a periodic update of data. If the app had
 * never registered any query, it is included in a list of 'registered'
 * applications, which contains all of them that have registered for something */
void register_query(int fd, app_query_t *query, long interval);

/* Unregisters a query given the request id */
void unregister_query(int fd, uint32_t req_id);

/* Unregisters an application, therefore all its queries. This could happen if
 * an app disconnects or closes abruptly */
void unregister_application(int fd);

void serve_query(int fd, app_query_t *query);

#endif /* APPLICATION_MANAGER_H_ */
