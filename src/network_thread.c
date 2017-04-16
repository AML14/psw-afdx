#include <sys/types.h>
#include <sys/socket.h>

#include "queue.h"

static int net_sockfd;

void init_net(struct sockaddr_in server_addr)
{
    if((net_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        /* Manage error */
    }
    else
    {
        if (bind(net_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        {
            /* Manage error */
        }
    }
}

void *network_thread(void *arg)
{
    nqnode *node;
    ssize_t rcv_size;
    socklen_t slen;
    /* Use sync mutex together with elaboration_thread */

    while(1)
    {
        node = next_nqnode_in();
        slen = sizeof(struct sockaddr_in);
        rcv_size = recvfrom(net_sockfd, (void *)(&(node->packet)),
                sizeof(afdx_packet_t), 0,
                (struct sockaddr *)(&(node->source)), &slen);
        if (rcv_size < 0)
        {
            /* Manage error */
        }
        else
        {
            /* Rise sync mutex for elaboration_thread to be able to get a node */
        }
    }
}
