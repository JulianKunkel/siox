/**
 * @file WorkerPool.h Pool of workers
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef WORKER_POOL_H
#define WORKER_POOL_H

#include "Worker.h"

#include <algorithm>
#include <list>

/**
 * A worker pool manages a dynamic number of worker threads.
 */
class WorkerPool {
private:
	std::list<Worker> workers;
public:
	WorkerPool();
	WorkerPool(const int size);
	~WorkerPool();
	
	void resize(int new_size);
};

#endif
