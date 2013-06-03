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
	void start()
	{
		int rc = pthread_create(&thread, NULL, llamada, (void*) this);
		if (rc > 0) throw std::runtime_error("No puedo crearse el thread");
		correr = true;
	}
	/**
	 * @brief Joinea el thread
	 */
	void join()
	{
	        void *p;
	        int ret = pthread_join(thread, &p);
	        if (ret != 0) throw std::runtime_error("Fallo el join");
	}
	/**
	 * @brief Pone al thread a dormir
	 */
	virtual bool sleep(unsigned int segundos)
	{
		return (sleep(segundos) == 0);
	}
	/**
	 * @brief Le pide gentilmente al thread que se detenga
	 */
	virtual void stop(){}
	virtual ~Thread() {};
protected:
	/**
	 * @brief Funcion que sera llamada internamente para correr el thread
	 */
	virtual void ejecutar() = 0;
	/**
	 * @brief Funcion estatica necesaria para ejecutar al thread internamente
	 */
	static void* llamada(void* thread)
	{
		((Thread*)thread)->ejecutar();
		return NULL;
	}
	pthread_t thread;
	bool correr;
};


#endif /* THREADM_H_ */
