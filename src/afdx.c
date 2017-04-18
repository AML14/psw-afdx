#include "internal_data.h"
#include "simplog.h"
#include "globals.h"

intdata_t intdata;

void init_all()
{
	queue_mutex = sem_open("queue_mutex",O_CREAT,0600,1);
	nr_nodes_queue = sem_open("nr_nodes_queue",O_CREAT,0600,0);
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
