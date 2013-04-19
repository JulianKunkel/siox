#include <cerrno>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc/IPCServer.h"
#include "tcp/TCPServer.h"

#define DAEMON_NAME "sioxd"

static void terminate_daemon(int ecode)
{
	syslog(LOG_NOTICE, "Shutting down dæmon...");
	
	closelog();
	exit(ecode);
}


static void server()
{
// 	TCPServer s("localhost", "6677", 10);
	IPCServer s("/tmp/ipcsock", 10);
	s.run();
}


int main(int argc, char *argv[])
{
	setlogmask(LOG_UPTO(LOG_NOTICE));
	openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, 
		LOG_USER);
	
	syslog(LOG_NOTICE, "Starting dæmon...");
	
	pid_t pid, sid;
	pid = fork();
	
	if (pid < 0)
		exit(EXIT_FAILURE);
	
	if (pid > 0)
		exit(EXIT_SUCCESS);
	
	umask(0);
	
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	
	if ((chdir("/")) < 0)
		exit(EXIT_FAILURE);
	
// 	close(STDIN_FILENO);
// 	close(STDOUT_FILENO);
// 	close(STDERR_FILENO);
	
	signal(SIGQUIT, terminate_daemon);
	signal(SIGTERM, terminate_daemon);
	
	// Begin Server Code
	
	server();
	
	// End Server Code
	
	return 0;
}
