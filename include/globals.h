#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <semaphore.h>

/* Semaphores related to queue. To be initialized on an init function */
extern sem_t *sync_elab_net_mutex; // 0
extern sem_t *queue_mutex; // 1
extern sem_t *nr_nodes_queue; // 0
/* Semaphore to access the list of registered apps with registered queries.
 * To be initialized on an init function */
extern sem_t *registering_mutex; // 1

/* Initialization functions */
void init_net();
void init_elab();
void init_app_man();

#endif /* GLOBALS_H_ */
