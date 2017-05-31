#include <sys/select.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>

#include "application_manager.h"
#include "internal_data.h"
#include "globals.h"

#define SOCKET_PATH "/tmp/server"

static int am_sockfd; /* Socket in which we listen */

void init_app_man() {

    struct sockaddr_un am_socket_addr;

    sem_init(registering_mutex,0,1);
    if ((am_sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        /* Manage error */
    }
    else
    {
        memset(&am_socket_addr, 0, sizeof(am_socket_addr));
        am_socket_addr.sun_family = AF_UNIX;
        strcpy(am_socket_addr.sun_path, SOCKET_PATH);

        if (bind(am_sockfd, (struct sockaddr *)&am_socket_addr, SUN_LEN(&am_socket_addr)) < 0)
        {
            /* Manage error */
        }
    }
}

void *application_manager(void *arg)
{
    fd_set managed_fds, rfds, xfds; // rfds = Descriptors ready for reading
                                    // xfds = Descriptors with an exceptional condition pending
    int nfds; // Highest fd
    int fdcnt; // Number of managed fds
    int newfd;
    int fd;
    app_msg_t packet_msg;
    uint8_t msg_type;
    ssize_t pktlen;

    /* Set managed_fds, which will keep the copy of all fds we have interest in */
    FD_ZERO(&managed_fds);
    FD_SET(am_sockfd, &managed_fds);
    nfds = am_sockfd + 1;

    while(1)
    {
        rfds = managed_fds;
        xfds = managed_fds;
        fdcnt = select(nfds, &rfds, NULL, &xfds, NULL);
        if (FD_ISSET(am_sockfd, &rfds))
        {
            newfd = listen(am_sockfd, 5);
            if (newfd < 0)
            {
                /* Error accepting connection */
                /* Manage error */
            }
            else
            {
                FD_SET(newfd, &managed_fds);
                fdcnt--;
                if (newfd >= nfds)
                    nfds = newfd + 1;
            }
        }
        if (FD_ISSET(am_sockfd, &xfds))
        {
            /* Error in connection socket */
            /* Manage error */
        }
        for(fd = 0; fd < nfds && fdcnt > 0; fd++)
        {
            if (FD_ISSET(fd, &rfds) && fd != am_sockfd)
            {
                fdcnt--;
                /* Read and process packet */
                pktlen = read(fd, (void *)&packet_msg, sizeof(packet_msg));
                if (pktlen <= 0)
                {
                    if (pktlen < 0)
                    {
                        /* Error in application client */
                        /* Manage error */
                    }
                    /* Client closed  or error */
                    unregister_application(fd);
                    close(fd);
                    FD_CLR(fd, &managed_fds);
                }
                else
                {
                    /* Process message */
                    msg_type = packet_msg.msg_type;
                    switch (msg_type) {
                        case AFDX_MSG_TYPE_REQUEST:
                            serve_query(fd, &(packet_msg.query));
                            break;
                        case AFDX_MSG_TYPE_REGISTER:
                            register_query(fd, &(packet_msg.query), packet_msg.ms_to_update);
                            break;
                        case AFDX_MSG_TYPE_UNREGISTER:
                            unregister_query(fd, packet_msg.query.req_id);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        for(fd = 0; fd < nfds && fdcnt > 0; fd++)
        {
            if (FD_ISSET(fd, &xfds) && fd != am_sockfd)
            {
                fdcnt--;
                /* Error in the client */
                unregister_application(fd);
                close(fd);
                FD_CLR(fd, &managed_fds);
            }
        }
    }
}

void serve_query(int fd, app_query_t *query)
{
    int data_sent;
    int data_id;
    int engine_id;
    app_reply_t reply_msg;

    reply_msg.req_id = query->req_id;
    data_id = query->data_id;
    switch (data_id) {
        case ENGINE_THROTTLE:
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].throttle;
        case ENGINE_REAL_THRUST:
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].real_thrust;
        case ENGINE_OIL_TEMP:
            engine_id = query->engine_id;
            reply_msg.i16 = intdata.engines[engine_id].oil_temp;
        case ENGINE_OIL_PRESS:
            engine_id = query->engine_id;
            reply_msg.i16 = intdata.engines[engine_id].oil_press;
        case ENGINE_FUEL_PRESS:
            engine_id = query->engine_id;
            reply_msg.i16 = intdata.engines[engine_id].fuel_press;
        case ENGINE_STATUS:
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].status;
        case ENGINE_FLAGS:
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].engine_flags;
        case ATTITUDE_ALTITUDE:
            reply_msg.i32 = intdata.attitude.altitude;
        case ATTITUDE_ROLL:
            reply_msg.i16 = intdata.attitude.roll;
        case ATTITUDE_PITCH:
            reply_msg.i16 = intdata.attitude.pitch;
        case ATTITUDE_YAW:
            reply_msg.i16 = intdata.attitude.yaw;
        case SPEEDS_AOA:
            reply_msg.i16 = intdata.speeds.aoa;
        case SPEEDS_AIRSPEED:
            reply_msg.i16 = intdata.speeds.airspeed;
    }
    data_sent = write(fd, &reply_msg, sizeof(reply_msg));
    if (data_sent < 0)
    {
        /* Manage error */
    }
}
