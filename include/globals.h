#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <semaphore.h>
#include <pthread.h>

/* Semaphore and mutexes related to queue. To be initialized on an init function */
extern sem_t *nr_nodes_queue; // 0
/* Condition to wake up elaboration thread */
/* Semaphore to access the list of registered apps with registered queries.
 * To be initialized on an init function */
extern sem_t *registering_mutex; // 1

void wakeup_elaboration_thread();

/* Initialization functions */
void init_queue();
void init_net(char *net_server_ipaddr, int net_server_port);
void init_elab();
void init_app_man();
void init_sched();

#endif /* GLOBALS_H_ */
