#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "simplog.h"

#include "queue.h"
#include "globals.h"

static int net_sockfd;
static pthread_t net_thread;

long start_net_thread();

void init_net(char *net_server_ipaddr, int net_server_port)
{
    struct sockaddr_in net_server_addr;
    int broadcast_enable;

    simplog.writeLog(SIMPLOG_DEBUG,"%s\n", net_server_ipaddr);
    simplog.writeLog(SIMPLOG_DEBUG,"%d\n", net_server_port);

    memset(&net_server_addr, 0, sizeof(net_server_addr));
    net_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, net_server_ipaddr, &net_server_addr.sin_addr);
    net_server_addr.sin_port = htons(net_server_port);

    if((net_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        /* Manage error */
    }
    else
    {
        broadcast_enable = 1;
        if (setsockopt(net_sockfd, SOL_SOCKET, SO_BROADCAST,
            &broadcast_enable, sizeof(broadcast_enable)) < 0)
        {
            /* Manage error */
            simplog.writeLog(SIMPLOG_DEBUG,"Error al establecer opciones\n");
        }
        else
        {
            simplog.writeLog(SIMPLOG_DEBUG,"Sin error al establecer opciones\n");
        }

        simplog.writeLog(SIMPLOG_DEBUG,"Network thread: sin error al crear socket\n");
        if (bind(net_sockfd, (struct sockaddr *) &net_server_addr, sizeof(net_server_addr)) < 0)
        {
            /* Manage error */
            simplog.writeLog(SIMPLOG_DEBUG,"Network thread: error al hacer bind\n");
        }
        else
        {
            simplog.writeLog(SIMPLOG_DEBUG,"Network thread: sin error al hacer bind\n");
        }
    }

    start_net_thread();
}

static void *network_thread(void *arg)
{
    nqnode_t *node;
    ssize_t rcv_size;
    socklen_t slen;
    char buff[20]; // To print the update time (debugging)

    while(1)
    {
        simplog.writeLog(SIMPLOG_DEBUG,"Network thread\n");
        node = next_nqnode_in();
        if (node != NULL) // Queue is not full
        {
            slen = sizeof(struct sockaddr_in);
            simplog.writeLog(SIMPLOG_DEBUG,"Network thread: antes de recibir\n");
            rcv_size = recvfrom(net_sockfd, (void *)(&(node->packet)),
                    sizeof(afdx_packet_t), 0,
                    (struct sockaddr *)(&(node->source)), &slen);
            simplog.writeLog(SIMPLOG_DEBUG,"Network thread: después de recibir\n");
            if (rcv_size < 0)
            {
                /* Manage error */
            }
            else
            {
                /* Set reception time */
                node->last_update = time(NULL);
                strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&(node->last_update)));
                simplog.writeLog(SIMPLOG_DEBUG,"%s\n", buff);
                /* Wake up elaboration_thread to be able to get a node */
                wakeup_elaboration_thread();
            }
        }
    }
}

long start_net_thread()
{
    return pthread_create(&net_thread, NULL, network_thread, NULL);
}
