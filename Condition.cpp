#include "stdafx.h"

#include "Condition.h"
#include "DayTime.h"

using namespace Answer;

Condition::Condition(Mutex &mutex)
: m_mutex(mutex)
{
	pthread_cond_init(&m_cond, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&m_cond);
}

void Condition::wait()
{
	pthread_cond_wait(&m_cond, m_mutex.getPthreadMutex());
}

void Condition::timedwait(int32_t seconds)
{
	timespec abstime = {};
	abstime.tv_sec = DayTime::now() + seconds;

	pthread_cond_timedwait(&m_cond, m_mutex.getPthreadMutex(), &abstime);
}

void Condition::notify()
{
	pthread_cond_signal(&m_cond);
}

void Condition::broadcast()
{
	pthread_cond_broadcast(&m_cond);
}
