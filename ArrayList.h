#pragma once

#include "Logger.h"
#include "SpinLock.h"
#include "SafeQueue.h"

namespace Answer
{
	template<typename T>
	class ArrayList
	{
	public:
		ArrayList(size_t capacity)
			: m_capacity(capacity), m_items(new T[capacity])
		{
			for (size_t i = 0; i < capacity; ++i)
			{
				m_items[i] = 0;
				m_freelist.push(i+1);
			}
		}

		~ArrayList()
		{
			delete[] m_items;
			m_items = NULL;
			m_capacity = 0;
		}

	public:
		int add(T pt)
		{
			if (pt == 0)
			{
				return -1;
			}

			int pos = m_freelist.pop();
			if (pos == 0)
			{
				return -1;
			}

			m_items[pos-1] = pt;
			return pos-1;
		}

		bool remove(int pos, T val)
		{
			if (pos < 0 || pos >= m_capacity)
			{
				return false;
			}

			m_lock.lock();
			m_items[pos] = NULL;
			m_freelist.push(pos+1);
			m_lock.unlock();
			return true;
		}

		T find(int pos)
		{
			if (pos <= 0 || pos >= m_capacity)
			{
				return T(0);
			}

			m_lock.lock();
			T val = m_items[pos];
			m_lock.unlock();

			return val;
		}

	private:
		int m_capacity;
		T *m_items;
		SpinLock m_lock;
		SafeQueue<int> m_freelist;
	};
}

