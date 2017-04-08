#include <stdint.h>



#define AFDX_PACKET_TYPE_UNKNOWN 0
#define AFDX_PACKET_TYPE_ADIRU   1
#define AFDX_PACKET_TYPE_ENGINE  2


typedef struct afdx_adiru_t { /* Air Data Inertial Reference Unit */
	int16_t airspeed;
	int32_t altitude;

	int16_t aoa; /* angle of attack */

	int16_t roll;
	int16_t pitch;
	int16_t yaw;
} afdx_adiru_t;

typedef struct afdx_engine_t {

	uint8_t engine_id;

	uint8_t throttle;	/* 0 - 100 */
	uint8_t real_thrust;	/* 0 - 100 */
	int16_t oil_temp;
	int16_t oil_press;
	int16_t fuel_press;

	uint8_t flag_status : 2;  /* 00 - stop,
                                     01 - starting,
                                     10 - running
                                     11 - shutdown in progress
                                   */
	uint8_t flag_reverse      : 1;
	uint8_t flag_fire         : 1;
	uint8_t flag_maintainance : 1;
} afdx_engine_t;

typedef struct afdx_packet_t {

	uint8_t identifier;

	union {
		afdx_adiru_t adiru;
		afdx_engine_t engine;
	} payload;

} afdx_packet_t;
