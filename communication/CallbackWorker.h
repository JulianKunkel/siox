/**
 * @file CallbackWorker.h Callback Worker
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef CALLBACK_WORKER_H
#define CALLBACK_WORKER_H

#include "QueuedMessageConnection.h"
#include "Worker.h"

/**
 * A callback worker is a worker instance with one or more attached connections.
 */
class CallbackWorker : public Worker {
private:
	std::list<QueuedMessageConnection> connections;
public:
	CallbackWorker();
	~CallbackWorker();
	
	void attach_connection(const QueuedMessageConnection &connection);
	void process_queue();
};

#endif
