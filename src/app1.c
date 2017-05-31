#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>

#include "network_protocol.h"
#include "application_protocol.h"

#define SOCKET_PATH "/tmp/server"


int main() {
    struct sockaddr_un serv_addr;
    int src_sock;
    int data_sent;

    app_msg_t message;

    if ((src_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        /* Manage error */
    }
    else
    {
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path, SOCKET_PATH);

        if (connect(src_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        {
            /* Manage error */
        }
        else
        {
            /* This is a test. Fill message with example data */
            message.msg_type = AFDX_MSG_TYPE_REQUEST;
            message.query.req_id = 12345678;
            message.query.data_id = SPEEDS_AIRSPEED;

            data_sent = write(src_sock, &message, sizeof(message));
            if (data_sent < 0)
            {
                /* Manage error */
            }
        }

        if (close(src_sock) < 0)
        {
            /* Manage error */
        }
    }
    return 0;
}
