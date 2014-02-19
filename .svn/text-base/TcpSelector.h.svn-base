#pragma once

#include "Thread.h"

#include <list>
#include <map>

namespace Answer
{
	class TcpClient;

	class ANSWER_EXPORT_SYMBOL TcpSelector
		: public Answer::Thread
	{
	public:
		TcpSelector(int32_t timeout = -1);
		virtual ~TcpSelector();

	protected:
		virtual void run();

	public:
		void stop();
		void add(TcpClient *pConn, MultiplexorMask mask);
		void remove(TcpClient *pConn);

	private:
		void handleRemovedFds();
		socket_t fillFdSet();
		void dispatchEvent();

	private:
		typedef std::map<socket_t, TcpClient*> SockConnMap;
		SockConnMap m_conns;
		typedef std::list<socket_t> SockList;
		SockList m_removelist;
		Mutex m_lock;
		fd_set m_rfds;
		fd_set m_wfds;
		timeval m_timeout;
		bool m_running;
	};
}

