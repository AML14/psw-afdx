#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "network_protocol.h"
#include "application_protocol.h"

#define SOCKET_PATH "/tmp/afdx_socket"


int main() {
    struct sockaddr_un serv_addr;
    int src_sock;
    int data_sent;
    int data_recvd;

    app_msg_t message;
    app_reply_t reply;

    int i;

    if ((src_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket error");
        exit(1);
    }
    else
    {
        printf("No socket error\n");
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path, SOCKET_PATH);

        if (connect(src_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("Error in connect");
            exit(1);
        }
        else
        {
            printf("No error connecting\n");
            /* This is a test. Fill message with example data */
            message.msg_type = AFDX_MSG_TYPE_REGISTER;
            message.ms_to_update = 2000;
            message.query.req_id = 87654321;
            message.query.data_id = SPEEDS_AOA;

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                perror("Error sending data");
                exit(1);
            }

            usleep(3650000);
            message.msg_type = AFDX_MSG_TYPE_REGISTER;
            message.ms_to_update = 500;
            message.query.req_id = 876543210;
            message.query.data_id = ENGINE_FLAGS;
            message.query.engine_id = 1;

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                perror("Error sending data");
                exit(1);
            }

            for (i=0; i<500; i++)
            {
                data_recvd = read(src_sock, &reply, sizeof(reply));
                if (data_recvd < 0) {
                    perror("Error reading data");
                    exit(1);
                }
                else {
                    /* Process message */
                    printf("%d\n", reply.req_id);
                    if (reply.req_id == 87654321)
                        printf("%d\n", reply.i16);
                    else
                        printf("%d\n", reply.u8);
                }
            }

            message.msg_type = AFDX_MSG_TYPE_UNREGISTER;
            message.query.req_id = 87654321; // Same request id

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                perror("Error sending data");
                exit(1);
            }

            message.query.req_id = 876543210; // Same request id

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                perror("Error sending data");
                exit(1);
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
