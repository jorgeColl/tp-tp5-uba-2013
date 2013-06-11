#ifndef THREADM_H_
#define THREADM_H_

#include "pthread.h"
#include <unistd.h> //Sleep
#include <stdexcept>

class Thread
{
public:
	/**
	 * @brief Crea un nuevo hilo de ejecucion y lo pone a correr
	 */
	void start();

	/**
	 * @brief Joinea el thread
	 */
	void join();

	/**
	 * @brief Le pide gentilmente al thread que se detenga
	 */
	virtual void stop();
	virtual ~Thread();
	bool correr;
protected:
	/**
	 * @brief Funcion que sera llamada internamente para correr el thread
	 */
	virtual void ejecutar() = 0;
	/**
	 * @brief Funcion estatica necesaria para ejecutar al thread internamente
	 */
	static void* llamada(void* thread);
	pthread_t thread;

};


#endif /* THREADM_H_ */
