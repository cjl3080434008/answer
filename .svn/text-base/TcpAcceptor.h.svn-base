#pragma once

#include "ObjectPool.h"
#include "TcpConnection.h"
#include "Thread.h"

#include <vector>

namespace Answer
{
	class InetAddress;
	class TcpConnection;
	class TcpMultiplexor;
	class TcpProcessor;
	class TcpServer;

	class TcpAcceptor
		: public Thread
	{
	public:
		TcpAcceptor(TcpServer *server, unsigned int initConnCount, unsigned int maxConnCount, bool bCompress, bool bCheckPolicy);
		~TcpAcceptor();

	protected:
		virtual void run();

	public:
		int startListen(int port, int backlog, int iothreadnum, int processthreadnum, unsigned int packSize, unsigned int initPackCount, unsigned int maxPackCount);
		void stopListen();
		
		void pushConn(TcpConnection *conn);

	private:
		void onAccept(socket_t fd, const InetAddress &addr);

	private:
		TcpServer *m_server;
		bool m_bCompress;
		bool m_bCheckPolicy;

		bool m_listening;
		socket_t m_listenFd;
		int m_listenPort;

		ObjectPool<TcpConnection> m_connPool;
		std::vector<TcpMultiplexor*> m_multiplexors;
		std::vector<TcpMultiplexor*>::size_type m_nextMultiplexor;
		std::vector<TcpProcessor*> m_processors;
		std::vector<TcpProcessor*>::size_type m_nextProcessor;
	};
}

