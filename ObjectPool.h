#pragma once

#include "Mutex.h"
#include "SafeQueue.h"

namespace Answer
{
	template<typename T>
	class ObjectPool
	{
	public:
		ObjectPool(unsigned int initCount, unsigned int maxCount)
			: m_initCount(initCount), m_maxCount(maxCount)
		{
			for (unsigned int i = 0; i != initCount; ++i)
			{
				push(new T);
			}
		}

		~ObjectPool()
		{
			T *pVal = NULL;
			while ((pVal = m_pool.pop()) != NULL)
			{
				delete pVal;
			}
		}

		T* pop()
		{
			T *pVal = m_pool.pop();
			if (pVal == NULL)
			{
				pVal = new T;
			}
			pVal->reset();

			return pVal;
		}

		void push(T *pVal)
		{
			if (pVal != NULL)
			{
				pVal->reset();
				m_pool.push(pVal);

				if (m_pool.size() > m_maxCount)
				{
					delete m_pool.pop();
				}
			}
		}

	private:
		SafeQueue<T*> m_pool;

		int m_initCount;
		int m_maxCount;
	};
}

