#include "queue.h"
#include "internal_data.h"
#include "globals.h"

void init_elab() {
    sync_elab_net_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    wakeup_elab = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
}

void *elaboration_thread(void *arg)
{
    nqnode_t *node = NULL;
    uint8_t id_engine;
    uint8_t id_packet;

    while (1) {
        /* Lock sync mutex */
        /* Thread must block here until there are messages */
        pthread_cond_wait(&wakeup_elab, &sync_elab_net_mutex);
        node = next_nqnode_out();
        id_packet = node->packet.identifier;
        switch (id_packet) {
            case AFDX_PACKET_TYPE_ADIRU:
                intdata.attitude.last_update = node->last_update;
                intdata.attitude.altitude = node->packet.payload.adiru.altitude;
                intdata.attitude.roll = node->packet.payload.adiru.roll;
                intdata.attitude.pitch = node->packet.payload.adiru.pitch;
                intdata.attitude.yaw = node->packet.payload.adiru.yaw;
                intdata.speeds.last_update = node->last_update;
                intdata.speeds.aoa = node->packet.payload.adiru.aoa;
                intdata.speeds.airspeed = node->packet.payload.adiru.airspeed;
                break;
            case AFDX_PACKET_TYPE_ENGINE:
                id_engine = node->packet.payload.engine.engine_id;
                intdata.engines[id_engine].last_update = node->last_update;
                intdata.engines[id_engine].throttle = node->packet.payload.engine.throttle;
                intdata.engines[id_engine].real_thrust = node->packet.payload.engine.real_thrust;
                intdata.engines[id_engine].oil_temp = node->packet.payload.engine.oil_temp;
                intdata.engines[id_engine].oil_press = node->packet.payload.engine.oil_press;
                intdata.engines[id_engine].fuel_press = node->packet.payload.engine.fuel_press;
                intdata.engines[id_engine].status = node->packet.payload.engine.flag_status;
                /* Engine flags:
                 *     0: No flags
                 *     1: R
                 *     2: OF
                 *     3: R+OF
                 *     4: M
                 *     5: R+M
                 *     6: OF+M
                 *     7: R+OF+M
                 */
                intdata.engines[id_engine].engine_flags =
                        node->packet.payload.engine.flag_reverse*ENGINE_F_REVERSE+
                        node->packet.payload.engine.flag_fire*ENGINE_F_ON_FIRE+
                        node->packet.payload.engine.flag_maintainance*ENGINE_F_MAINT;
                break;
            default:
                break;
        }
        dispose_last_nqnode(); // Update out pointer
    }
}
