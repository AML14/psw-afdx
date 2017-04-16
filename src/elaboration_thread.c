#include "queue.h"
#include "internal_data.h"


void *elaboration_thread(void *arg)
{
    nqnode *node = NULL;
    uint8_t id_engine;
    uint8_t id_packet;

    while (1) {
        /* Lower sync mutex */
        sem_wait(sync_elab_net_mutex);
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
                intdata.engines[id_engine].throttle = node->packet.payload.engine.throttle;
                intdata.engines[id_engine].real_thrust = node->packet.payload.engine.real_thrust;
                intdata.engines[id_engine].oil_temp = node->packet.payload.engine.oil_temp;
                intdata.engines[id_engine].oil_press = node->packet.payload.engine.oil_press;
                intdata.engines[id_engine].fuel_press = node->packet.payload.engine.fuel_press;
                intdata.engines[id_engine].status = node->packet.payload.engine.flag_status;
                /* Engine flags:
                 *     - Reverse flag:        bit 0
                 *     - Fire flag:           bit 1
                 *     - Maintainance flag:   bit 2
                 */
                intdata.engines[id_engine].engine_flags =
                        node->packet.payload.engine.flag_reverse |
                        node->packet.payload.engine.flag_fire << 1 |
                        node->packet.payload.engine.flag_maintainance << 2;
                break;
            default:
                break;
        }
        dispose_last_nqnode(); /* Update out pointer */
    }
}
