#pragma once

#include <pthread.h>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL SpinLock
	{
	public:
		SpinLock();
		~SpinLock();

	private:
		DISALLOW_COPY_AND_ASSIGN(SpinLock);

	public:
		void lock();
		void unlock();

	private:
		pthread_spinlock_t m_spinlock;
	};

	class ANSWER_EXPORT_SYMBOL SpinLockGuard
	{
	public:
		SpinLockGuard(SpinLock &spinlock);
		~SpinLockGuard();

	private:
		DISALLOW_COPY_AND_ASSIGN(SpinLockGuard);

	private:
		SpinLock &m_spinlock;
	};
}

