#include "stdafx.h"

#include "DayTime.h"
#include "Logger.h"
#include "SockOpts.h"
#include "TcpConnection.h"
#include "TcpMultiplexor.h"

using namespace Answer;
using namespace std;

#ifdef LIBANSWER_WINDOWS
TcpMultiplexor::TcpMultiplexor(int maxClients, int timeout /* = -1 */)
	: m_running(false)
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

TcpMultiplexor::~TcpMultiplexor()
{

}

void TcpMultiplexor::run()
{
	m_running = true;
	while (m_running)
	{
		handleRemovedFds();

		fd_set *rfds = NULL;
		fd_set *wfds = NULL;

		getFdSets(rfds, wfds);
		if (rfds == NULL && wfds == NULL)
		{
			DayTime::sleep(m_timeout.tv_sec*1000+m_timeout.tv_usec/1000);
			continue;
		}

		int ret = select(0, rfds, wfds, NULL, &m_timeout);

		if (ret > 0)
		{
			dispatchEvent(rfds, wfds);
		}
		else if (ret == 0)
		{
			continue;
		}
		else
		{
			LOG_ERROR("TcpMultiplexor::run() select return %d, errno %d\n", ret, WSAGetLastError());
			m_running = false;
		}
	}
}

void TcpMultiplexor::stop()
{
	m_running = false;
	join();
}

void TcpMultiplexor::add(TcpConnection *pConn, MultiplexorMask mask)
{
	if (pConn != NULL)
	{
		pConn->enableMask(mask);
		pConn->onConnect();
		MutexGuard lock(m_lock);
		m_conns.insert(SockConnMap::value_type(pConn->getFd(), pConn));
	}
}

void TcpMultiplexor::remove(TcpConnection *pConn)
{
	if (pConn != NULL)
	{
		MutexGuard lock(m_lock);
		m_removelist.push_back(pConn->getFd());
		pConn->onDisconnect();
	}
}

void TcpMultiplexor::getFdSets(fd_set *&rfds, fd_set *&wfds)
{
	FD_ZERO(&m_rfds);
	FD_ZERO(&m_wfds);

	MutexGuard lock(m_lock);
	for (SockConnMap::iterator it = m_conns.begin(); it != m_conns.end(); ++it)
	{
		TcpConnection *pConn = it->second;
		if (pConn != NULL)
		{
			socket_t fd = pConn->getFd();
			if (fd == INVALID_SOCKET)
			{
				continue;
			}
			int mask = pConn->getMask();
			if (mask & MASK_READ)
			{
				FD_SET(fd, &m_rfds);
			}

			if (mask & MASK_WRITE)
			{
				FD_SET(fd, &m_wfds);
			}
		}
	}

	getFdSet(m_rfds, rfds);
	getFdSet(m_wfds, wfds);
}

void TcpMultiplexor::getFdSet(fd_set &in, fd_set *&out)
{
	if (!in.fd_count)
	{
		out = NULL;
	}
	else
	{
		out = &in;
	}
}

void TcpMultiplexor::handleRemovedFds()
{
	MutexGuard lock(m_lock);
	for (SockList::iterator it = m_removelist.begin(); it != m_removelist.end(); ++it)
	{
		m_conns.erase(*it);
	}
	m_removelist.clear();
}

void TcpMultiplexor::dispatchEvent(const fd_set *rfds, const fd_set *wfds)
{
	if (rfds != NULL)
	{
		dispatchEvent(rfds, MASK_READ);
	}

	if (wfds != NULL)
	{
		dispatchEvent(wfds, MASK_WRITE);
	}
}

void TcpMultiplexor::dispatchEvent(const fd_set *fds, MultiplexorMask mask)
{
	if (fds != NULL)
	{
		for (unsigned int i = 0; i < fds->fd_count; i++)
		{
			SOCKET fd = fds->fd_array[i];
			TcpConnection *pConn = NULL;
			{
				MutexGuard lock(m_lock);
				SockConnMap::iterator it = m_conns.find(fd);
				if (it != m_conns.end())
				{
					pConn = it->second;
				}			
			}

			if (pConn != NULL)
			{
				switch (mask)
				{
				case MASK_READ:
					if (pConn->onRecv() == -1)
					{
						remove(pConn);
					}
					break;
				case MASK_WRITE:
					{
						int err = pConn->onSend();
						while (err > 0 && pConn->canNextSend())
						{
							err = pConn->onSend();
						}
						if (err == -1)
						{
							remove(pConn);
						}
					}
					break;
				default:
					LOG_ERROR("TcpMultiplexor::dispatchEvent with wrong mask %d\n", mask);
					break;
				}
			}
		}
	}
}
#endif

#ifdef LIBANSWER_LINUX
TcpMultiplexor::TcpMultiplexor(int maxClients, int timeout /* = -1 */)
{
	m_maxClients = maxClients;

	if (timeout == -1)
	{
		m_timeout = 1000;
	}
	else
	{
		m_timeout = timeout;
	}

	m_epfd = -1;
}

TcpMultiplexor::~TcpMultiplexor()
{

}

void TcpMultiplexor::run()
{
	m_epfd = epoll_create(m_maxClients);
	if (m_epfd == -1)
	{
		LOG_ERROR("TcpMultiplexor::run, epoll_create return -1 with m_maxClients = %d\n", m_maxClients);
		return;
	}

	epoll_event *events = new epoll_event[m_maxClients];
	bzero(events, sizeof(epoll_event)*m_maxClients);

	m_running = true;
	while (m_running)
	{
		int nfds = epoll_wait(m_epfd, events, m_maxClients, m_timeout);
		for (int i = 0; i < nfds; ++i)
		{
			TcpConnection *pConn = static_cast<TcpConnection*>(events[i].data.ptr);
			if (pConn != NULL)
			{
				int event = events[i].events;

				if (event&EPOLLOUT)
				{
					int err = pConn->onSend();
					while (err > 0 && pConn->canNextSend())
					{
						err = pConn->onSend();
					}
					if (err == -1)
					{
						remove(pConn);
					}
				}

				if (event&EPOLLIN)
				{
					if (pConn->onRecv() == -1)
					{
						remove(pConn);
					}
				}

				if ( (event&EPOLLIN)==0 && (event&EPOLLOUT)==0 )
				{
					LOG_ERROR("TcpMultiplexor::run, TcpConnection from ip %s, event %d, disconnect\n", pConn->getIP().c_str(), event);
					remove(pConn);
				}
			}
		}
	}
}

void TcpMultiplexor::stop()
{
	m_running = false;
	join();
}

void TcpMultiplexor::add(TcpConnection *pConn, MultiplexorMask mask)
{
	if (pConn != NULL)
	{
		epoll_event ev = {};
		ev.events = 0;
		ev.data.ptr = pConn;
		int err = epoll_ctl(m_epfd, EPOLL_CTL_ADD, pConn->getFd(), &ev);
		if (err == 0)
		{
			pConn->setEpfd(m_epfd);
			pConn->enableMask(mask);
			pConn->onConnect();
		}
		else
		{
			LOG_ERROR("TcpMultiplexor::add epoll_ctl with epfd %d, fd %d, return %d, errno %d\n", m_epfd, pConn->getFd(), err, errno);
		}
	}
}

void TcpMultiplexor::remove(TcpConnection *pConn)
{
	if (pConn != NULL)
	{
		epoll_event ev = {};
		epoll_ctl(m_epfd, EPOLL_CTL_DEL, pConn->getFd(), &ev);
		pConn->onDisconnect();
	}
}
#endif

