#ifndef APPLICATION_PROTOCOL_H_
#define APPLICATION_PROTOCOL_H_

#include <stdint.h>

#define AFDX_MSG_TYPE_UNKNOWN 0
#define AFDX_MSG_TYPE_REQUEST 1
#define AFDX_MSG_TYPE_REGISTER 2
#define AFDX_MSG_TYPE_UNREGISTER 3


typedef enum data_id_t {
	ENGINE_THROTTLE,
	ENGINE_REAL_THRUST,
	ENGINE_OIL_TEMP,
	ENGINE_OIL_PRESS,
    ENGINE_FUEL_PRESS,
    ENGINE_STATUS,
    ENGINE_FLAGS,
    ATTITUDE_ALTITUDE,
    ATTITUDE_ROLL,
    ATTITUDE_PITCH,
    ATTITUDE_YAW,
    SPEEDS_AOA,
    SPEEDS_AIRSPEED
} data_id_t;

typedef struct app_query_s {
    uint32_t req_id;  // Client defined request id
    union
    {
        uint8_t engine_id; // Optional field
    };
    union
    {
        data_id_t data_id; // Specific data requested
    };
} app_query_t;

typedef struct app_msg_s {
    uint8_t msg_type; // Request, register or unregister
    union
    {
        uint16_t ms_to_update; // Interval between updates. Not present if request type
    };
    app_query_t query;
} app_msg_t;

typedef struct app_reply_s {
    uint32_t req_id;
    union
    {
        uint8_t u8;
    };
    union
    {
        int16_t i16;
    };
    union
    {
        int32_t i32;
    };
} app_reply_t;

#endif /* APPLICATION_PROTOCOL_H_ */
