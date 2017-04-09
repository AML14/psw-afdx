#include <stdint.h>
#include <time.h>

#define NR_ENGINES 4

typedef enum engine_status_t {
	STOP,
	STARTING,
	RUNNING,
	SHUTDOWN_IN_PROGRESS
} engine_status_t;

#define ENGINE_F_REVERSE 1
#define ENGINE_F_ON_FIRE 2
#define ENGINE_F_MAINT   4

typedef struct intdata_engine_t {
	time_t last_update; 	/* 0 means invalid data */

	uint8_t throttle;
	uint8_t real_thrust;
	int16_t oil_temp;
	int16_t oil_press;
	int16_t fuel_press;

	engine_status_t status;
	uint8_t engine_flags;

} intdata_engine_t;

typedef struct intdata_attitude_t {
	time_t last_update; 	/* 0 means invalid data */

	int32_t altitude;

	int16_t roll;
	int16_t pitch;
	int16_t yaw;


} intdata_attitude_t;

typedef struct intdata_speeds_t {
	time_t last_update; 	/* 0 means invalid data */

	int16_t aoa; /* angle of attack */
	int16_t airspeed;
	/* ... */

} intdata_speeds_t;

typedef struct intdata_t {
	intdata_engine_t engines[NR_ENGINES];
	intdata_attitude_t attitude;
	intdata_speeds_t speeds;

} intdata_t;

extern intdata_t intdata;

