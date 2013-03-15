/**
 * @file Worker.h SIOX Worker
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef WORKER_H
#define WORKER_H

/**
 * A worker is an independent thread with an associated work-function. 
 */
class Worker {
private:
	void *work_function;
public:
	Worker();
	~Worker();
	
	void set_work_function(void *work_function);
	
	void work();
	void start();
	void stop();
};

#endif
