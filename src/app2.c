#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>

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

    printf("Hola\n");

    if ((src_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        /* Manage error */
        printf("Socket error\n");
    }
    else
    {
        printf("Sin error de socket\n");
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path, SOCKET_PATH);

        if (connect(src_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        {
            /* Manage error */
            printf("Error al conectarse\n");
            printf("%s\n", strerror(errno));
        }
        else
        {
            printf("Sin error al conectarse\n");
            /* This is a test. Fill message with example data */
            message.msg_type = AFDX_MSG_TYPE_REQUEST;
            message.query.req_id = 23456789;
            message.query.engine_id = 2;
            message.query.data_id = ENGINE_THROTTLE;

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                printf("Error al enviar los datos\n");
                /* Manage error */
            }

            data_recvd = read(src_sock, &reply, sizeof(reply));
            if (data_recvd < 0) {
                printf("Error al leer los datos\n");
                /* Manage error */
            }
            else
            {
                /* Process message */
                printf("%d\n", reply.u8);
            }
        }

        if (close(src_sock) < 0)
        {
            /* Manage error */
        }
    }
    return 0;
}
