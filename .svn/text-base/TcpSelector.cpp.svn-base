#include "stdafx.h"

#include "DayTime.h"
#include "Logger.h"
#include "Mutex.h"
#include "SockOpts.h"
#include "TcpClient.h"
#include "TcpSelector.h"

using namespace Answer;

TcpSelector::TcpSelector(int32_t timeout /* = -1 */)
{
	if (timeout == -1)
	{
		m_timeout.tv_sec = 0;
		m_timeout.tv_usec = 50*1000;//50ms
	}
	else 
	{
		m_timeout.tv_sec = timeout / 1000;
		m_timeout.tv_usec = timeout % 1000 * 1000;
	}

	m_running = false;
}

TcpSelector::~TcpSelector()
{

}

void TcpSelector::run()
{
	m_running = true;
	while (m_running)
	{
		handleRemovedFds();

		socket_t maxfd = fillFdSet();

		int ret = select(maxfd+1, &m_rfds, &m_wfds, NULL, &m_timeout);

		if (ret > 0)
		{
			dispatchEvent();
		}
		else if (ret == 0)
		{
			continue;
		}
		else
		{
			LOG_ERROR("TcpSelector::run() select return %d, errno %d\n", ret, WSAGetLastError());
			m_running = false;
		}

		DayTime::sleep(1);
	}
}

void TcpSelector::stop()
{
	m_running = false;
	join();
}

void TcpSelector::add(TcpClient *pConn, MultiplexorMask mask)
{
	if (pConn != NULL)
	{
		MutexGuard lock(m_lock);
		m_conns.insert(SockConnMap::value_type(pConn->getFd(), pConn));
	}
}

void TcpSelector::remove(TcpClient *pConn)
{
	if (pConn != NULL)
	{
		m_removelist.push_back(pConn->getFd());
		pConn->onDisconnect();
	}
}

void TcpSelector::handleRemovedFds()
{
	MutexGuard lock(m_lock);
	for (SockList::iterator it = m_removelist.begin(); it != m_removelist.end(); ++it)
	{
		m_conns.erase(*it);
	}
	m_removelist.clear();
}

socket_t TcpSelector::fillFdSet()
{
	FD_ZERO(&m_rfds);
	FD_ZERO(&m_wfds);

	socket_t maxfd = 0;

	MutexGuard lock(m_lock);
	for (SockConnMap::iterator it = m_conns.begin(); it != m_conns.end(); ++it)
	{
		TcpClient *pConn = it->second;
		if (pConn != NULL)
		{
			socket_t fd = pConn->getFd();
			if (fd != INVALID_SOCKET)
			{
				FD_SET(fd, &m_rfds);
				if (pConn->needSend())
				{
					FD_SET(fd, &m_wfds);
				}
				if (fd > maxfd)
				{
					maxfd = fd;
				}
			}
		}
	}

	return maxfd;
}

void TcpSelector::dispatchEvent()
{
	MutexGuard lock(m_lock);
	for (SockConnMap::iterator it = m_conns.begin(); it != m_conns.end(); ++it)
	{
		if (FD_ISSET(it->first, &m_rfds))
		{
			if (it->second->onRecv() == -1)
			{
				remove(it->second);
			}
		}

		if (FD_ISSET(it->first, &m_wfds))
		{
			if (it->second->onSend() == -1)
			{
				remove(it->second);
			}
		}
	}
}
