#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include "simplog.h"

#include "queue.h"
#include "internal_data.h"
#include "simplog.h"
#include "globals.h"

intdata_t intdata;

void init_all(char *net_server_ipaddr, int net_server_port)
{
	init_queue();
	init_net(net_server_ipaddr, net_server_port);
	init_elab();
	init_app_man();
    init_sched();
}

void daemonize()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/Users/Alberto/psw-afdx");

    /* Close all open file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    sleep(1);
}

int main(int argc, char *argv[]) {
    int i;

    //daemonize();

    simplog.setLogSilentMode(true);
    simplog.setLogFile("debug_afdx.log");
    simplog.flushLog();
    simplog.setLogDebugLevel(SIMPLOG_DEBUG);

	simplog.writeLog( SIMPLOG_INFO, "Starting..." );
	simplog.writeLog( SIMPLOG_DEBUG, "Creating the internal data structure..." );
	/* Set the internal data structures as invalid */
	for (i=0; i<NR_ENGINES; i++) {
		intdata.engines[i].last_update = 0;
	}
	intdata.attitude.last_update = 0;
	intdata.speeds.last_update = 0;

    init_all(argv[1], atoi(argv[2]));

    simplog.writeLog(SIMPLOG_DEBUG,"All threads initialized");

    while(1){}

	return 0;
}
