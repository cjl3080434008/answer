#pragma once

#include "Mutex.h"

#include <pthread.h>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL Condition
	{
	public:
		Condition(Mutex &mutex);
		~Condition();

	private:
		DISALLOW_COPY_AND_ASSIGN(Condition);

	public:
		void wait();
		void timedwait(int32_t seconds);
		void notify();
		void broadcast();

	private:
		Mutex &m_mutex;
		pthread_cond_t m_cond;
	};
}
