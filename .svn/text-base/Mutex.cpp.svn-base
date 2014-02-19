#include "stdafx.h"
#include "Mutex.h"

using namespace Answer;

Mutex::Mutex()
{
	/*	If need recursive mutex, use this code block...
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);			
	pthread_mutex_init(&m_mutex, &attr);
	*/
	pthread_mutex_init(&m_mutex, NULL);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
	pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

pthread_mutex_t* Mutex::getPthreadMutex()
{
	return &m_mutex;
}

MutexGuard::MutexGuard(Mutex &mutex)
: m_mutex(mutex)
{
	m_mutex.lock();
}

MutexGuard::~MutexGuard()
{
	m_mutex.unlock();
}

