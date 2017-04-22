#include "internal_data.h"
#include "simplog.h"
#include "globals.h"

intdata_t intdata;

void init_all()
{
	queue_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	sem_init(nr_nodes_queue,0,0);
	init_net();
	init_elab();
	init_app_man();
}

int main() {
	init_all();

	simplog.writeLog( SIMPLOG_INFO, "Starting..." );

	simplog.writeLog( SIMPLOG_DEBUG, "Creating the internal data structure..." );
	/* Set the internal data structures as invalid */
	for (int i=0; i<NR_ENGINES; i++) {
		intdata.engines[i].last_update = 0;
	}
	intdata.attitude.last_update = 0;
	intdata.speeds.last_update = 0;

	return 0;
}
