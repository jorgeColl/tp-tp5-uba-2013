#include <pthread.h>
#include <stdexcept>
#include <unistd.h> //Sleep
#include "common_thread.h"

Thread::Thread() : corriendo(true) {}

Thread::Thread(const Thread &otroThread) : corriendo(otroThread.corriendo) {}

#include <iostream>
using namespace std;

void Thread::start()
{
	int ret = pthread_create(&cThread, 0, callback, this);
	if (ret != 0) throw std::runtime_error("Fallo al crear el thread.");
}

void Thread::stop()
{
	corriendo = false;
}

void Thread::join()
{
	void *p;
	int ret = pthread_join(cThread, &p);
	if (ret != 0) throw std::runtime_error("Fallo el join");
}

bool Thread::sleep(unsigned int segundos)
{
	return (sleep(segundos) == 0);
}
