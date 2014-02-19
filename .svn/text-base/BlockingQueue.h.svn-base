#pragma once

#include "Condition.h"
#include "Mutex.h"
#include "SafeQueue.h"

namespace Answer
{
	template<typename T>
	class BlockingQueue
	{
	public:
		BlockingQueue()
			: m_mutex(), m_cond(m_mutex)
		{

		}
		~BlockingQueue()
		{
	
		}

		void push(const T &val)
		{
			MutexGuard lock(m_mutex);
			m_queue.push(val);
			m_cond.notify();
		}

		T pop()
		{
			MutexGuard lock(m_mutex);
			while (m_queue.size() <= 0)
			{
				m_cond.wait();
			}

			T val = m_queue.pop();
			return val;
		}

		int size()
		{
			return m_queue.size();
		}

	private:
		Mutex m_mutex;
		Condition m_cond;
		SafeQueue<T> m_queue;
	};
}

