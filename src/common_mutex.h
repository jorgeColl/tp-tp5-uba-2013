#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>
#include <iostream>

/**
 * @class Mutex commonMutex.h "commonMutex.h"
 * @brief Encapsulamiento al mutex normal de C
 */
class Mutex
{
	public:
	  Mutex();
	  ~Mutex();
	  friend class Lock;
	private:
	  pthread_mutex_t mutex;
	  pthread_cond_t conditionVariable;
	  Mutex (const Mutex &otroMutex);
	  void wait();
	  void signal();
	  void broadcast();
	  void lock();
	  void unlock();
};

/**
 * @class Lock commonMutex.h "commonMutex.h"
 * @brief Permite utilizar el mutex de forma mas abstracta
 */
class Lock
{
	public:
	  Lock(Mutex &mutex);
	  ~Lock();
	  void wait();
	  void signal();
	  void broadcast();
	private:
	  Mutex &mutex;
	  Lock(const Lock &otroLock);
};


#endif
