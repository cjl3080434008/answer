#pragma once

#include "Mutex.h"

#include <list>

namespace Answer
{
	template <typename T>
	class SafeQueue
	{
	public:
		SafeQueue()
			: m_size(0)
		{

		}

		~SafeQueue()
		{

		}

	public:
		T front()
		{
			Answer::MutexGuard lock(m_lock);
			if (m_queue.empty())
			{
				return T(0);
			}
			else
			{
				return m_queue.front();
			}
		}

		T pop()
		{
			Answer::MutexGuard lock(m_lock);
			if (!m_queue.empty())
			{
				T ret = m_queue.front();
				m_queue.pop_front();
				--m_size;
				return ret;
			}
			else
			{
				return T(0);
			}
		}

		void push(T val)
		{
			Answer::MutexGuard lock(m_lock);
			m_queue.push_back(val);
			++m_size;
		}

		long size()
		{
			Answer::MutexGuard lock(m_lock);
			return m_size;
		}


	private:
		std::list<T> m_queue;
		int32_t m_size;
		Answer::Mutex m_lock;
	};

	class NetPacket;
	typedef SafeQueue<NetPacket*> NetPacketList;
}

