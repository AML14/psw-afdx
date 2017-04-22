#include <sys/select.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "application_manager.h"
#include "globals.h"

static int sockfd; /* Socket in which we listen */

void init_app_man() {
    sem_init(registering_mutex,0,1);
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
    ssize_t pktlen;

    /* Set managed_fds, which will keep the copy of all fds we have interest in */
    FD_ZERO(&managed_fds);
    FD_SET(sockfd, &managed_fds);
    nfds = sockfd + 1;

    while(1)
    {
        rfds = managed_fds;
        xfds = managed_fds;
        fdcnt = select(nfds, &rfds, NULL, &xfds, NULL);
        if (FD_ISSET(sockfd, &rfds))
        {
            newfd = listen(sockfd, 5);
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
        if (FD_ISSET(sockfd, &xfds))
        {
            /* Error in connection socket */
            /* Manage error */
        }
        for(fd = 0; fd < nfds && fdcnt > 0; fd++)
        {
            if (FD_ISSET(fd, &rfds) && fd != sockfd)
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
                }
            }
        }
        for(fd = 0; fd < nfds && fdcnt > 0; fd++)
        {
            if (FD_ISSET(fd, &xfds) && fd != sockfd)
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
