/*
 * treadM.h
 *
 *  Created on: 29/04/2013
 *      Author: jorge
 */

#ifndef THREADM_H_
#define THREADM_H_
#include "pthread.h"
class ThreadM {
protected:
	pthread_t* thread;
public:
	/**@brief ejecuta al hijo */
	static void* ejecut_hijo(void* hijo) {
		ThreadM* hijito = (ThreadM*) hijo;
		hijito->ejecutar();
		return NULL;
	}
	/**@brief crea un nuevo hilo de ejecucion */
	void start() {
		int rc = pthread_create(&thread[0], NULL, ThreadM::ejecut_hijo,
				(void*) this);
		if (rc > 0) {
			//cout<<"THREAD MALO!!"<<endl;
			throw 33;
		}
	}
	virtual void stop(){}

	virtual ~ThreadM() {};

	virtual void ejecutar() = 0;
};


#endif /* THREADM_H_ */
