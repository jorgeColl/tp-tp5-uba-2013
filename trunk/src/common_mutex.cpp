#include "common_mutex.h"

Mutex::Mutex()
{
	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&conditionVariable, 0);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&conditionVariable);
}

//Mutex::Mutex (const Mutex &otroMutex) {}

void Mutex::wait()
{
	pthread_cond_wait(&conditionVariable, &mutex);
}

void Mutex::signal()
{
	pthread_cond_signal(&conditionVariable);
}

void Mutex::broadcast()
{
	pthread_cond_broadcast(&conditionVariable);
}

void Mutex::lock()
{
	pthread_mutex_lock(&mutex);
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&mutex);
}

//Lock

Lock::Lock(Mutex &mutex) : mutex(mutex)
{
	mutex.lock();
}

Lock::~Lock()
{
	mutex.unlock();
}

void Lock::wait()
{
	mutex.wait();
}

void Lock::signal()
{
	mutex.signal();
}

void Lock::broadcast()
{
	mutex.broadcast();
}

Lock::Lock(const Lock &otroLock) :mutex(otroLock.mutex) {}
