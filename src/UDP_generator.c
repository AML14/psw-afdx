#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "network_protocol.h"

/* Predefined port and broadcast address */
#define AFDX_PORT 15000
#define AFDX_BROADCAST_ADDRESS "192.168.0.255"


int main() {
    struct sockaddr_in bc_addr;
    int src_sock;
    int socket_options;
    int broadcast_enable;
    int data_sent;
    afdx_packet_t net_packets[10];
    int i;
    int engine_id = 0;
    int general_flag = 0;
    if ((src_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        /* Manage error */
    }
    else
    {
        broadcast_enable = 1;
        socket_options = setsockopt(src_sock, SOL_SOCKET, SO_BROADCAST,
            &broadcast_enable, sizeof(broadcast_enable));
        /* This is a test. Fill net_packets with example data */
        for (i=0; i<10; i++)
        {
            if (i==3 || i==8) // 2 unknown packets
            {
                net_packets[i].identifier = AFDX_PACKET_TYPE_UNKNOWN;
            }
            else if ((i+2)%2) // 4 adiru packets
            {
                net_packets[i].identifier = AFDX_PACKET_TYPE_ADIRU;
                net_packets[i].payload.adiru.airspeed = 20+5*i;
                net_packets[i].payload.adiru.altitude = 3000+100*i;
                net_packets[i].payload.adiru.aoa = 15+i;
                net_packets[i].payload.adiru.roll = 5+i;
                net_packets[i].payload.adiru.pitch = 3+2*i;
                net_packets[i].payload.adiru.yaw = 1+3*i;
            }
            else // 4 engine packets, one for every engine
            {
                net_packets[i].identifier = AFDX_PACKET_TYPE_ENGINE;
                net_packets[i].payload.engine.engine_id = engine_id;
                net_packets[i].payload.engine.throttle = 10+i;
                net_packets[i].payload.engine.real_thrust = 5+2*i;
                net_packets[i].payload.engine.oil_temp = 40+10*i;
                net_packets[i].payload.engine.oil_press = 20+5*i;
                net_packets[i].payload.engine.fuel_press = 30+4*i;
                net_packets[i].payload.engine.flag_status = engine_id++;
                net_packets[i].payload.engine.flag_reverse = general_flag;
                net_packets[i].payload.engine.flag_fire = !general_flag;
                net_packets[i].payload.engine.flag_maintainance = general_flag;
                general_flag = !general_flag;
            }
        }

        memset(&bc_addr, 0, sizeof(bc_addr));
        bc_addr.sin_family = AF_INET;
        inet_pton(AF_INET, AFDX_BROADCAST_ADDRESS, &bc_addr.sin_addr);
        bc_addr.sin_port = htons(AFDX_PORT);

        /* Send one packet every 100 milliseconds */
        for (i=0; i<10; i++)
        {
            data_sent = sendto(src_sock, &net_packets[i], sizeof(net_packets[i]),
                0, (struct sockaddr *)&bc_addr, sizeof(bc_addr));
            if (data_sent < 0)
            {
                /* Manage error */
            }
            else
            {
                usleep(1000*100);
            }
        }

        if (close(src_sock) < 0)
        {
            /* Manage error */
        }
    }
    return 0;
}
