#pragma once

#include <pthread.h>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL Thread
	{
	public:
		Thread();
		virtual	~Thread();

	private:
		DISALLOW_COPY_AND_ASSIGN(Thread);

	public:
		int start();
		void join();

	protected:
		virtual void run();

	private:
		static void* threadEntry(void *param);

	private:
		pthread_t m_thread;
	};
}

