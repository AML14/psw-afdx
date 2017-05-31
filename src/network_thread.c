#include <sys/types.h>
#include <sys/socket.h>

#include "queue.h"
#include "globals.h"

static int net_sockfd;

void init_net(struct sockaddr_in net_server_addr)
{
    if((net_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        /* Manage error */
    }
    else
    {
        if (bind(net_sockfd, (struct sockaddr *) &net_server_addr, sizeof(net_server_addr)) < 0)
        {
            /* Manage error */
        }
    }
}

void *network_thread(void *arg)
{
    nqnode_t *node;
    ssize_t rcv_size;
    socklen_t slen;

    while(1)
    {
        node = next_nqnode_in();
        if (node != NULL) // Queue is not full
        {
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
                /* Set reception time */
                node->last_update = time(NULL);
                /* Wake up elaboration_thread to be able to get a node */
                pthread_cond_signal(&wakeup_elab);
            }
        }
    }
}
