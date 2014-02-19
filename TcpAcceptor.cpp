#include "stdafx.h"

#include "Logger.h"
#include "InetAddress.h"
#include "SockOpts.h"
#include "TcpAcceptor.h"
#include "TcpConnection.h"
#include "TcpMultiplexor.h"
#include "TcpProcessor.h"

using namespace Answer;

TcpAcceptor::TcpAcceptor(TcpServer *server, unsigned int initConnCount, unsigned int maxConnCount, bool bCompress, bool bCheckPolicy)
	: m_server(server), m_bCompress(bCompress), m_bCheckPolicy(bCheckPolicy), m_listening(false), m_listenFd(INVALID_SOCKET), m_listenPort(0), m_connPool(initConnCount, maxConnCount)
{
	
}

TcpAcceptor::~TcpAcceptor()
{

}

void TcpAcceptor::run()
{
	m_listening = true;
	while (m_listening)
	{
		sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		socklen_t len = sizeof(addr);

		socket_t fd = accept(m_listenFd, sockaddr_cast(&addr), &len);
		if (fd == INVALID_SOCKET)
		{
			LOG_ERROR("TcpAcceptor::run, accept error, return INVALID_SOCKET, errno=%d\n", WSAGetLastError());
		}
		else
		{
			InetAddress inetaddr(addr);
			LOG_DEBUG("TcpAcceptor::run, accept socket = %d, ip = %s, port = %d\n", fd, inetaddr.getIP().c_str(), static_cast<int>(inetaddr.getPort()));
			onAccept(fd, inetaddr);
		}
	}
}

int TcpAcceptor::startListen(int port, int backlog, int iothreadnum, int processthreadnum, unsigned int packSize, unsigned int initPackCount, unsigned int maxPackCount)
{
	if (m_listening)
	{
		return ERR_ANSWER_SOCKET;
	}

	do 
	{
		m_listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (m_listenFd == INVALID_SOCKET)
		{
			LOG_ERROR("TcpAcceptor::startListen, socket error, errno=%d\n", WSAGetLastError());
			break;
		}

		setreuseaddr(m_listenFd);
		setreuseport(m_listenFd);

		InetAddress addr(static_cast<uint16_t>(port));
		LOG_DEBUG("try to bind to port %d\n", port);
		if (::bind(m_listenFd, sockaddr_cast(&addr.getSockAddr()), addr.getAddrLen()) != 0)
		{
			LOG_ERROR("TcpAcceptor::startListen, bind error, errno=%d\n", WSAGetLastError());
			break;
		}

		if (::listen(m_listenFd, backlog) != 0)
		{
			LOG_ERROR("TcpAcceptor::startListen, listen error, errno=%d\n", WSAGetLastError());
			break;
		}

		if (iothreadnum <= 0 || processthreadnum <= 0)
		{
			LOG_ERROR("TcpAcceptor::startListen, can't create thread, iothreadnum %d, processthreadnum %d\n", iothreadnum, processthreadnum);
			break;
		}

		for (int i = 0; i < iothreadnum; ++i)
		{
			TcpMultiplexor *pMultiplexor = new TcpMultiplexor(10000);
			if (pMultiplexor->start() != ERR_ANSWER_OK)
			{
				LOG_ERROR("TcpAcceptor::startListen start multiplexor thread failed\n");
				continue;
			}
			m_multiplexors.push_back(pMultiplexor);
		}
		if (m_multiplexors.empty())
		{
			LOG_ERROR("TcpAcceptor::startListen no multiplexor thread created");
			break;
		}
		m_nextMultiplexor = 0;

		for (int i = 0; i < processthreadnum; ++i)
		{
			TcpProcessor *pProcessor = new TcpProcessor(this, m_server, packSize, initPackCount, maxPackCount);
			if (pProcessor->start() != ERR_ANSWER_OK)
			{
				LOG_ERROR("TcpAcceptor::startListen start processor thread failed\n");
				continue;
			}
			m_processors.push_back(pProcessor);
		}
		if (m_processors.empty())
		{
			LOG_ERROR("TcpAcceptor::startListen no processor thread created");
			break;
		}
		m_nextProcessor = 0;

		sockaddr_in listenaddr;
		bzero(&listenaddr, sizeof(listenaddr));
		socklen_t len = sizeof(listenaddr);
		getsockname(m_listenFd, sockaddr_cast(&listenaddr), &len);
		m_listenPort = ntohs(listenaddr.sin_port);
		LOG_INFO("TcpAcceptor::startListen start listening on port %d\n", m_listenPort);

		m_listening = true;
		start();

		return ERR_ANSWER_OK;

	}while (false);

	if (m_listenFd != INVALID_SOCKET)
	{
		Answer::close(m_listenFd);
		m_listenFd = INVALID_SOCKET;
	}

	return ERR_ANSWER_SOCKET;

}

void TcpAcceptor::stopListen()
{
	m_listening = false;

	if (m_listenFd != INVALID_SOCKET)
	{
		Answer::halt(m_listenFd);
		Answer::close(m_listenFd);
		m_listenFd = INVALID_SOCKET;
	}

	m_listenPort = 0;

	for (std::vector<TcpMultiplexor*>::iterator it = m_multiplexors.begin(); it != m_multiplexors.end(); ++it)
	{
		(*it)->stop();
	}

	for (std::vector<TcpProcessor*>::iterator it = m_processors.begin(); it != m_processors.end(); ++it)
	{
		(*it)->stop();
	}

	join();
}

void TcpAcceptor::pushConn(TcpConnection *conn)
{
	if (conn != NULL)
	{
		m_connPool.push(conn);
	}
}

void TcpAcceptor::onAccept(socket_t fd, const InetAddress &addr)
{
	TcpConnection *pConn = m_connPool.pop();
	if (pConn == NULL)
	{
		LOG_ERROR("TcpAcceptor::onAccept, m_connPool.pop() return NULL\n");
		Answer::close(fd);
		return;
	}

	pConn->setCompress(m_bCompress);
	pConn->setCheckPolicy(m_bCheckPolicy);

	pConn->bind(fd, addr, m_processors[m_nextProcessor]);
	++m_nextProcessor;
	if (m_nextProcessor >= m_processors.size())
	{
		m_nextProcessor = 0;
	}

	m_multiplexors[m_nextMultiplexor]->add(pConn, MASK_READ);
	++m_nextMultiplexor;
	if (m_nextMultiplexor >= m_multiplexors.size())
	{
		m_nextMultiplexor = 0;
	}
	pConn->enableMask(MASK_READ);
}

