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
            message.msg_type = AFDX_MSG_TYPE_REQUEST;
            message.query.req_id = 12345678;
            message.query.data_id = SPEEDS_AOA;

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                perror("Error sending data");
                exit(1);
            }

            data_recvd = read(src_sock, &reply, sizeof(reply));
            if (data_recvd < 0) {
                perror("Error reading data");
                exit(1);
            }
            else
            {
                /* Process message */
                printf("%d\n", reply.i16);
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
