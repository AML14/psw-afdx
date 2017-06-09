#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "network_protocol.h"

#define NUM_PACKETS 20

int main(int argc, char *argv[]) {
    struct sockaddr_in bc_addr;
    int src_sock;
    int broadcast_enable;
    int data_sent;
    afdx_packet_t net_packets[NUM_PACKETS];
    int i;
    int engine_id = 0;
    int general_flag = 0;

    if ((src_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket error");
        exit(1);
    }
    else
    {
        broadcast_enable = 1;
        if (setsockopt(src_sock, SOL_SOCKET, SO_BROADCAST,
            &broadcast_enable, sizeof(broadcast_enable)) < 0)
        {
            perror("Error setting socket options");
            exit(1);
        }
        else
        {
            printf("No error setting options\n");
        }
        /* This is a test. Fill net_packets with example data */
        for (i=0; i<NUM_PACKETS; i++)
        {
            if (i==3 || i==8 || i==11 || i==16) // 4 unknown packets
            {
                net_packets[i].identifier = AFDX_PACKET_TYPE_UNKNOWN;
            }
            else if ((i+2)%2) // 8 adiru packets
            {
                net_packets[i].identifier = AFDX_PACKET_TYPE_ADIRU;
                net_packets[i].payload.adiru.airspeed = 20+5*i;
                net_packets[i].payload.adiru.altitude = 3000+100*i;
                net_packets[i].payload.adiru.aoa = 15+i;
                net_packets[i].payload.adiru.roll = 5+i;
                net_packets[i].payload.adiru.pitch = 3+2*i;
                net_packets[i].payload.adiru.yaw = 1+3*i;
            }
            else // 8 engine packets, one for every engine
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
                if (engine_id == 4)
                {
                    engine_id = 0;
                }
            }
        }

        memset(&bc_addr, 0, sizeof(bc_addr));
        bc_addr.sin_family = AF_INET;
        bc_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        bc_addr.sin_port = htons(atoi(argv[1]));

        /* Send one packet every 100 milliseconds */
        for (i=0; i<NUM_PACKETS; i++)
        {
            data_sent = sendto(src_sock, &net_packets[i], sizeof(net_packets[i]),
                0, (struct sockaddr *)&bc_addr, sizeof(bc_addr));
            if (data_sent < 0)
            {
                perror("Error sending data");
                exit(1);
            }
            else
            {
                printf("Packet %d sent\n", i+1);
                getc(stdin);
            }
        }

        if (close(src_sock) < 0)
        {
            perror("Error closing socket");
            exit(1);
        }
    }
    return 0;
}
