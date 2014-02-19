#include "stdafx.h"

#include "Logger.h"
#include "SockOpts.h"

using namespace Answer;

const sockaddr* Answer::sockaddr_cast(const sockaddr_in *addr)
{
	return (const sockaddr*)(addr);
}

sockaddr* Answer::sockaddr_cast(sockaddr_in *addr)
{
	return (sockaddr*)(addr);
}

int Answer::setkeepalive(socket_t fd, int idleseconds)
{
#ifdef LIBANSWER_WINDOWS
	const char chOpt = 1;  
	int err = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&chOpt, sizeof(chOpt));
	if (err == 0)
	{
		tcp_keepalive klive;  
		klive.onoff = 1;
		klive.keepalivetime = idleseconds*1000;//keepalivetime is millisecond
		klive.keepaliveinterval = 10 * 1000;//keepaliveinterval is millisecond
		tcp_keepalive kliveout;
		DWORD dwBytes = 0;
		err = WSAIoctl(fd, SIO_KEEPALIVE_VALS, &klive, sizeof(klive), &kliveout, sizeof(kliveout), &dwBytes, NULL, NULL);
	}

	return err;
#endif

#ifdef LIBANSWER_LINUX
	int keepalive = 1;
	int keepidle = idleseconds;
	int keepintvl = 10;
	int keepcnt = 5;

	int err = 0;

	err = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepalive, sizeof(keepalive));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setkeepalive\n");
	}
	err = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, (char *)&keepidle, sizeof(keepidle));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setkeepalive\n");
	}
	err = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL,(char *)&keepintvl, sizeof(keepintvl));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setkeepalive\n");
	}
	err = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,(char *)&keepcnt, sizeof(keepcnt));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setkeepalive\n");
	}

	return err;
#endif
}

int Answer::setnodelay(socket_t fd)
{
	socklen_t optval = 1;
	int err = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setnodelay\n");
	}
	return err;
}

int Answer::setnonblock(socket_t fd)
{
#ifdef LIBANSWER_WINDOWS
	u_long flag=1;
	return ioctlsocket(fd, FIONBIO, &flag);
#endif

#ifdef LIBANSWER_LINUX
	int opts = fcntl(fd, F_GETFL);
	opts = opts|O_NONBLOCK;
	int err = fcntl(fd, F_SETFL, opts);
	if (err == -1)
	{
		LOG_DEBUG("fcntl return -1 in Answer::setnonblock\n");
	}
	return err;
#endif
}

int Answer::setreuseaddr(socket_t fd)
{
#ifdef LIBANSWER_WINDOWS
	return 0; //make no sense in this framework in windows...
#endif

#ifdef LIBANSWER_LINUX
	socklen_t val = 1;
	int err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&val, sizeof(val));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setreuseaddr\n");
	}
	return err;
#endif
}

int Answer::setreuseport(socket_t fd)
{
#ifdef LIBANSWER_WINDOWS
	return 0; //make no sense in this framework in windows...
#endif

#ifdef LIBANSWER_LINUX
	socklen_t val = 1;
	int err = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (void*)&val, sizeof(val));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setreuseport\n");
	}
	return err;
#endif
}

int Answer::getreadbuffer(socket_t fd)
{
	socklen_t optval = 0;
	socklen_t optlen = sizeof(optval);
	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen);
	return optval;
}

int Answer::setreadbuffer(socket_t fd, socket_t size)
{
	int err = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(socket_t));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setreadbuffer\n");
	}
	return err;
}

int Answer::getwritebuffer(socket_t fd)
{
	socklen_t optval = 0;
	socklen_t optlen = sizeof(optval);
	getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen);
	return optval;
}

int Answer::setwritebuffer(socket_t fd, socket_t size)
{
	int err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(socket_t));
	if (err == SOCKET_ERROR)
	{
		LOG_DEBUG("setsockopt return SOCKET_ERROR in Answer::setwritebuffer\n");
	}
	return err;
}

int Answer::close(socket_t fd)
{
#ifdef LIBANSWER_WINDOWS
	return ::closesocket(fd);
#endif

#ifdef LIBANSWER_LINUX
	return ::close(fd);
#endif
}

int Answer::halt(socket_t fd)
{
#ifdef LIBANSWER_WINDOWS
	return ::shutdown(fd, SD_BOTH);
#endif

#ifdef LIBANSWER_LINUX
	return ::shutdown(fd, SHUT_RDWR);
#endif

}

