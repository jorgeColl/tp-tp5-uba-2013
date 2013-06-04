#include "common_thread.h"

void Thread::start()
{
	int rc = pthread_create(&thread, NULL, llamada, (void*) this);
	if (rc > 0) throw std::runtime_error("No puedo crearse el thread");
	correr = true;
}

void Thread::join()
{
		void *p;
		int ret = pthread_join(thread, &p);
		if (ret != 0) throw std::runtime_error("Fallo el join");
}

bool Thread::sleep(unsigned int segundos)
{
	return (sleep(segundos) == 0);
}

void Thread::stop()
{
	correr = false;
}

Thread::~Thread() {}

void* Thread::llamada(void* thread)
{
	((Thread*)thread)->ejecutar();
	return NULL;
}
