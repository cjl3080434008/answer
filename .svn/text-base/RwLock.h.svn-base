#pragma once

#include <pthread.h>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL RwLock
	{
	public:
		RwLock();
		~RwLock();

	private:
		DISALLOW_COPY_AND_ASSIGN(RwLock);

	public:
		void rdlock();
		void wrlock();
		void unlock();

	private:
		pthread_rwlock_t m_rwlock;
	};

	class ANSWER_EXPORT_SYMBOL RwLockRdGuard
	{
	public:
		RwLockRdGuard(RwLock &rwlock);
		~RwLockRdGuard();

	private:
		DISALLOW_COPY_AND_ASSIGN(RwLockRdGuard);

	private:
		RwLock &m_rwlock;
	};

	class ANSWER_EXPORT_SYMBOL RwLockWrGuard
	{
	public:
		RwLockWrGuard(RwLock &rwlock);
		~RwLockWrGuard();

	private:
		DISALLOW_COPY_AND_ASSIGN(RwLockWrGuard);

	private:
		RwLock &m_rwlock;
	};
}
