#pragma once

#include <pthread.h>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL Mutex
	{
	public:
		Mutex();
		~Mutex();

	private:
		DISALLOW_COPY_AND_ASSIGN(Mutex);

	public:
		void lock();
		void unlock();
		pthread_mutex_t* getPthreadMutex();

	private:
		pthread_mutex_t m_mutex;
	};

	class ANSWER_EXPORT_SYMBOL MutexGuard
	{
	public:
		MutexGuard(Mutex &mutex);
		~MutexGuard();

	private:
		DISALLOW_COPY_AND_ASSIGN(MutexGuard);

	private:
		Mutex &m_mutex;
	};
}

