#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>
#include <iostream>

/**
 * @class Mutex common_mutex.h
 * @brief Encapsulamiento al mutex normal de C
 * Se pretende que se use mediante el Lock, por eso la mayoria de los metodos son privados.
 */
class Mutex
{
	public:
	  Mutex();
	  ~Mutex();
	  friend class Lock;
	  Mutex (const Mutex &otroMutex);
	  /**@brief Pone al mutex a esperar una senial */
	  void wait();
	  /**@brief Pasa la senial a otro thread que este esperando */
	  void signal();
	  /**@brief Pasa la senial a todos los threads que esten esperando */
	  void broadcast();
	  /**@brief Lockea el mutex o espera que se libere si ya estaba lockeado */
	  void lock();
	  /**@brief Libera el mutex */
	  void unlock();
	  pthread_mutex_t mutex;
	  pthread_cond_t conditionVariable;
};

/**
 * @class Lock common_mutex.h
 * @brief Permite utilizar el mutex de forma mas abstracta
 */
class Lock
{
	public:
	  Lock(Mutex &mutex);
	  Lock(Mutex& mutex, bool trabar);
	  ~Lock();
	  /**@brief Pone al mutex a esperar una senial */
	  void wait();
	  /**@brief Pasa la senial a otro thread que este esperando */
	  void signal();
	  /**@brief Lockea el mutex o espera que se libere si ya estaba lockeado */
	  void broadcast();
	private:
	  Mutex &mutex;
	  Lock(const Lock &otroLock);
};


#endif
