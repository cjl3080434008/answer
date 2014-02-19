#include "stdafx.h"

#include "RwLock.h"

using namespace Answer;

RwLock::RwLock()
{
	pthread_rwlock_init(&m_rwlock, NULL);
}

RwLock::~RwLock()
{
	pthread_rwlock_destroy(&m_rwlock);
}

void RwLock::rdlock()
{
	pthread_rwlock_rdlock(&m_rwlock);
}

void RwLock::wrlock()
{
	pthread_rwlock_wrlock(&m_rwlock);
}

void RwLock::unlock()
{
	pthread_rwlock_unlock(&m_rwlock);
}

RwLockRdGuard::RwLockRdGuard(RwLock &rwlock)
: m_rwlock(rwlock)
{
	m_rwlock.rdlock();
}

RwLockRdGuard::~RwLockRdGuard()
{
	m_rwlock.unlock();
}

RwLockWrGuard::RwLockWrGuard(RwLock &rwlock)
: m_rwlock(rwlock)
{
	m_rwlock.wrlock();
}

RwLockWrGuard::~RwLockWrGuard()
{
	m_rwlock.unlock();
}

