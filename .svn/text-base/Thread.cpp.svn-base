#include "stdafx.h"

#include "Thread.h"

using namespace Answer;

Thread::Thread()
{

}

Thread::~Thread()
{

}

int Thread::start()
{
	int err = pthread_create(&m_thread, NULL, Thread::threadEntry, this);
	if (err == 0)
	{
		return ERR_ANSWER_OK;
	}
	else
	{
		return err;
	}
}

void Thread::join()
{
	pthread_join(m_thread, NULL);
}

void Thread::run()
{

}

void* Thread::threadEntry(void *param)
{
	Thread *pThread = static_cast<Thread*>(param);
	if (pThread != NULL)
	{
		pThread->run();
	}

	return NULL;
}

