#include <sys/select.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include "simplog.h"

#include "application_manager.h"
#include "internal_data.h"
#include "globals.h"

#define SOCKET_PATH "/tmp/afdx_socket"

static int am_sockfd; /* Socket in which we listen */
static pthread_t app_man_thread;

long start_app_man_thread();

void init_app_man() {

    struct sockaddr_un am_socket_addr;
    am_socket_addr.sun_family = AF_UNIX;
    strcpy(am_socket_addr.sun_path, SOCKET_PATH);

    sem_init(registering_mutex,0,1);
    if ((am_sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        simplog.writeLog(SIMPLOG_DEBUG,"Error al crear el socket UNIX\n");
        /* Manage error */
    }
    else
    {
        unlink(SOCKET_PATH);
        simplog.writeLog(SIMPLOG_DEBUG,"Sin error al crear el socket\n");
        if (bind(am_sockfd, (struct sockaddr *)&am_socket_addr, sizeof(am_socket_addr)) < 0)
        {
            simplog.writeLog(SIMPLOG_DEBUG,"Error al hacer bind al path\n");
            /* Manage error */
        }
        else
        {
            simplog.writeLog(SIMPLOG_DEBUG,"Sin error al hacer bind al path\n");
            if (listen(am_sockfd, 5) < 0)
            {
                simplog.writeLog(SIMPLOG_DEBUG,"Error en el listen\n");
                /* Manage error */
            }
        }
    }

    start_app_man_thread();
}

void *application_manager(void *arg)
{
    fd_set managed_fds, rfds, xfds; // rfds = Descriptors ready for reading
                                    // xfds = Descriptors with an exceptional condition pending
    int nfds; // Highest fd
    int fdcnt; // Number of managed fds
    int newfd;
    int fd;
    struct sockaddr_un remote;
    socklen_t remote_len;
    app_msg_t packet_msg;
    uint8_t msg_type;
    ssize_t pktlen;

    /* Set managed_fds, which will keep the copy of all fds we have interest in */
    FD_ZERO(&managed_fds);
    FD_SET(am_sockfd, &managed_fds);
    nfds = am_sockfd + 1;

    while(1)
    {
        simplog.writeLog(SIMPLOG_DEBUG,"Application manager thread\n");
        rfds = managed_fds;
        xfds = managed_fds;
        simplog.writeLog(SIMPLOG_DEBUG,"Antes de select\n");
        fdcnt = select(nfds, &rfds, NULL, &xfds, NULL);
        simplog.writeLog(SIMPLOG_DEBUG,"Después de select\n");
        if (FD_ISSET(am_sockfd, &rfds))
        {
            simplog.writeLog(SIMPLOG_DEBUG,"Antes de accept\n");
            remote_len = sizeof(remote);
            if ((newfd = accept(am_sockfd, (struct sockaddr *)&remote, &remote_len)) < 0)
            {
                simplog.writeLog(SIMPLOG_DEBUG,"Error en accept\n");
                /* Manage error */
            }
            else {
                simplog.writeLog(SIMPLOG_DEBUG,"No error accepting connection\n");
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
                    simplog.writeLog(SIMPLOG_DEBUG,"Application unregistered\n");
                    unregister_application(fd);
                    close(fd);
                    simplog.writeLog(SIMPLOG_DEBUG,"Socket closed\n");
                    FD_CLR(fd, &managed_fds);
                }
                else
                {
                    /* Process message */
                    msg_type = packet_msg.msg_type;
                    simplog.writeLog(SIMPLOG_DEBUG,"%d\n", msg_type);
                    simplog.writeLog(SIMPLOG_DEBUG,"%d\n", packet_msg.query.req_id);
                    simplog.writeLog(SIMPLOG_DEBUG,"%d\n", packet_msg.query.data_id);
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
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 1\n");
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].throttle;
            break;
        case ENGINE_REAL_THRUST:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 1\n");
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].real_thrust;
            break;
        case ENGINE_OIL_TEMP:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 2\n");
            engine_id = query->engine_id;
            reply_msg.i16 = intdata.engines[engine_id].oil_temp;
            break;
        case ENGINE_OIL_PRESS:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 3\n");
            engine_id = query->engine_id;
            reply_msg.i16 = intdata.engines[engine_id].oil_press;
            break;
        case ENGINE_FUEL_PRESS:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 4\n");
            engine_id = query->engine_id;
            reply_msg.i16 = intdata.engines[engine_id].fuel_press;
            break;
        case ENGINE_STATUS:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 5\n");
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].status;
            break;
        case ENGINE_FLAGS:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 6\n");
            engine_id = query->engine_id;
            reply_msg.u8 = intdata.engines[engine_id].engine_flags;
            break;
        case ATTITUDE_ALTITUDE:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 7\n");
            reply_msg.i32 = intdata.attitude.altitude;
            break;
        case ATTITUDE_ROLL:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 8\n");
            reply_msg.i16 = intdata.attitude.roll;
            break;
        case ATTITUDE_PITCH:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 9\n");
            reply_msg.i16 = intdata.attitude.pitch;
            break;
        case ATTITUDE_YAW:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 10\n");
            reply_msg.i16 = intdata.attitude.yaw;
            break;
        case SPEEDS_AOA:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 11\n");
            reply_msg.i16 = intdata.speeds.aoa;
            break;
        case SPEEDS_AIRSPEED:
            simplog.writeLog(SIMPLOG_DEBUG,"Dentro del switch-case 12\n");
            reply_msg.i16 = intdata.speeds.airspeed;
            break;
    }
    data_sent = write(fd, &reply_msg, sizeof(reply_msg));
    if (data_sent < 0)
    {
        /* Manage error */
    }
}

long start_app_man_thread()
{
    return pthread_create(&app_man_thread, NULL, application_manager, NULL);
}
