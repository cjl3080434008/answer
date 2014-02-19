#include "stdafx.h"

#include "SpinLock.h"

using namespace Answer;

SpinLock::SpinLock()
{
	pthread_spin_init(&m_spinlock, 0);
}

SpinLock::~SpinLock()
{
	pthread_spin_destroy(&m_spinlock);
}

void SpinLock::lock()
{
	pthread_spin_lock(&m_spinlock);
}

void SpinLock::unlock()
{
	pthread_spin_unlock(&m_spinlock);
}

SpinLockGuard::SpinLockGuard(SpinLock &spinlock)
: m_spinlock(spinlock)
{
	m_spinlock.lock();
}

SpinLockGuard::~SpinLockGuard()
{
	m_spinlock.unlock();
}

