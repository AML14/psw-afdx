#include "internal_data.h"
#include "simplog.h"

intdata_t intdata;

int main() {

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
