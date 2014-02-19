#pragma once

#include "Mutex.h"
#include "Thread.h"

#include <list>
#include <map>

namespace Answer
{
	class TcpConnection;

	class TcpMultiplexor
		: public Thread
	{
	public:
		TcpMultiplexor(int maxClients, int timeout = -1);
		~TcpMultiplexor();

	protected:
		virtual void run();

	public:
		void stop();
		void add(TcpConnection *pConn, MultiplexorMask mask);
		void remove(TcpConnection *pConn);

#ifdef LIBANSWER_WINDOWS
	private:
		void getFdSets(fd_set *&rfds, fd_set *&wfds);
		void getFdSet(fd_set &in, fd_set *&out);
		void handleRemovedFds();
		void dispatchEvent(const fd_set *rfds, const fd_set *wfds);
		void dispatchEvent(const fd_set *fds, MultiplexorMask mask);

	private:
		typedef std::map<socket_t, TcpConnection*> SockConnMap;
		SockConnMap m_conns;
		typedef std::list<socket_t> SockList;
		SockList m_removelist;
		Mutex m_lock;
		fd_set m_rfds;
		fd_set m_wfds;
		timeval m_timeout;
		bool m_running;
#endif

#ifdef LIBANSWER_LINUX
	private:
		int m_maxClients;
		int m_timeout;
		int m_epfd;
		bool m_running;
#endif
	};
}

