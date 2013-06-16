#include "common_thread.h"

Thread::Thread() : correr(false) {}

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
