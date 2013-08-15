#include <cerrno>
#include <fcntl.h>
#include <inttypes.h>
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include <core/logger/SioxLogger.hpp>
#include <monitoring/transaction_system/PostgreSQLBackend.hpp>
#include <monitoring/transaction_system/TransactionBackend.hpp>
#include <monitoring/transaction_system/TransactionService.hpp>

#define SIOX_TS_LISTENER  "tcp://localhost:7000"
#define SIOX_DB_INFO "host = '127.0.0.1' port = '5432' user = 'postgres' password = 'siox.db.X916' dbname = 'siox-ts'"

TransactionService *ts;
Logger *logger;

void terminate_daemon(int ecode)
{
#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Terminating dæmon.");
#endif
	ts->stop();
	exit(ecode);
}


int transaction_daemon()
{
	ts = new TransactionService(SIOX_TS_LISTENER);
	
	TransactionBackend *be = new PostgreSQLBackend(SIOX_DB_INFO);
	ts->register_transaction_backend(be);
	
	ts->run();
	
	/** @todo better way to make the main thread idle. */
	pause();
	
	return 0;
}


int main(int argc, char *argv[])
{
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

	logger = new Logger();
	
	// Begin Server Code

	transaction_daemon();

	// End Server Code

	return 0;
}       
