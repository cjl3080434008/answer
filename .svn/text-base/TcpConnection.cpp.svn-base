#include "stdafx.h"

#include "DayTime.h"
#include "Logger.h"
#include "NetPacket.h"
#include "SockOpts.h"
#include "TcpConnection.h"
#include "TcpProcessor.h"
#include "Zip.h"

using namespace Answer;

TcpConnection::TcpConnection()
: m_fd(INVALID_SOCKET), m_processor(NULL), m_packet(NULL)
{
	m_recvBufferSize = CLIENT_RECV_BUFFER;
	m_recvBuffer = m_defaultRecvBuffer;

	m_sendBufferSize = CLIENT_SEND_BUFFER;
	m_sendBuffer = m_defaultSendBuffer;
}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::reset()
{
	if (m_fd != INVALID_SOCKET)
	{
		Answer::close(m_fd);
		m_fd = INVALID_SOCKET;
	}
	m_halt = false;

	m_addr = InetAddress();
	m_processor = NULL;

	bzero(m_recvBuffer, sizeof(char)*m_recvBufferSize);
	m_recvOffset = 0;

	if (m_packet != NULL)
	{
		m_packet->destroy();
	}
	m_decodeState = DECODE_HEAD;

	m_bCheckPackSize = true;
	m_bCheckHalt = true;
	m_bCheckPolicy = true;
	m_bCompress = true;

	m_maxPackSize = 0;

	NetPacket *packet = NULL;
	while ((packet = m_sendList.pop()) != NULL)
	{
		packet->destroy();
	}
	m_sendListByteLen = 0;

	m_parseLen = 0;

	bzero(m_sendBuffer, sizeof(char)*m_sendBufferSize);
	m_sendLen = 0;
	m_sendCursor = 0;

	m_mask = MASK_NONE;

	m_ptr = NULL;

#ifdef LIBANSWER_LINUX
	m_epfd = -1;
#endif
}

void TcpConnection::bind(socket_t fd, const InetAddress &addr, TcpProcessor *processor)
{
	m_fd = fd;
	m_addr = addr;
	m_processor = processor;
	if (m_maxPackSize < m_processor->getPacketSize())
	{
		m_maxPackSize = m_processor->getPacketSize();
	}
}

socket_t TcpConnection::getFd()
{
	return m_fd;
}

std::string TcpConnection::getIP()
{
	return m_addr.getIP();
}

void TcpConnection::setCheckPackSize(bool check)
{
	m_bCheckPackSize = check;
}

void TcpConnection::setCheckHalt(bool check)
{
	m_bCheckHalt = check;
}

void TcpConnection::setCheckPolicy(bool check)
{
	m_bCheckPolicy = check;
}

void TcpConnection::setCompress(bool compress)
{
	m_bCompress = compress;
}

int TcpConnection::getMask()
{
	return m_mask;
}

void TcpConnection::enableMask(MultiplexorMask mask)
{
	m_mask |= mask;

#ifdef LIBANSWER_LINUX
	epoll_event ev = {};
	ev.data.ptr = this;
	if ((m_mask&MASK_READ) != 0)
	{
		ev.events |= EPOLLIN;
	}
	if ((m_mask&MASK_WRITE) != 0)
	{
		ev.events |= EPOLLOUT;
	}
	epoll_ctl(m_epfd, EPOLL_CTL_MOD, m_fd, &ev);
#endif
}

void TcpConnection::disableMask(MultiplexorMask mask)
{
	m_mask &= ~mask;

#ifdef LIBANSWER_LINUX
	epoll_event ev = {};
	ev.data.ptr = this;
	if ((m_mask&MASK_READ) != 0)
	{
		ev.events |= EPOLLIN;
	}
	if ((m_mask&MASK_WRITE) != 0)
	{
		ev.events |= EPOLLOUT;
	}
	epoll_ctl(m_epfd, EPOLL_CTL_MOD, m_fd, &ev);
#endif
}

void TcpConnection::onConnect()
{
	Answer::setnonblock(m_fd);
	Answer::setnodelay(m_fd);
	Answer::setkeepalive(m_fd, 300);
	int rcvbuf = Answer::getreadbuffer(m_fd);
	int sndbuf = Answer::getwritebuffer(m_fd);
	LOG_DEBUG("TcpConnection::onConnect fd = %d with rcvbuf=%d, sndbuf=%d before set\n", m_fd, rcvbuf, sndbuf);
	Answer::setreadbuffer(m_fd, 32*1024);
	Answer::setwritebuffer(m_fd, 32*1024);
	rcvbuf = Answer::getreadbuffer(m_fd);
	sndbuf = Answer::getwritebuffer(m_fd);
	LOG_DEBUG("TcpConnection::onConnect fd = %d with rcvbuf=%d, sndbuf=%d after set\n", m_fd, rcvbuf, sndbuf);

	if (m_processor != NULL)
	{
		m_processor->postMessage(TCP_MSG_CONNECT, this, NULL);
	}
}

void TcpConnection::onDisconnect()
{
	LOG_DEBUG("TcpConnection::onDisconnect socket = %d, ip =%s, port = %d, this = %p\n", static_cast<int>(m_fd), m_addr.getIP().c_str(), static_cast<int>(m_addr.getPort()), this);

	if (m_fd == INVALID_SOCKET)
	{
		LOG_ERROR("TcpConnection::onDisconnect this = %p already disconnected\n", this);
	}

	Answer::close(m_fd);
	m_fd = INVALID_SOCKET;

	TcpProcessor *processor = m_processor;
	if (processor != NULL)
	{
		processor->postMessage(TCP_MSG_DISCONNECT, this, NULL);
	}
	m_processor = NULL;
}

int TcpConnection::onRecv()
{
	int error = ::recv(m_fd, m_recvBuffer+m_recvOffset, m_recvBufferSize-m_recvOffset, 0);
	if (error > 0)
	{
		m_recvOffset += error;
		if (m_recvOffset > m_recvBufferSize)
		{
			LOG_INFO("TcpConnection from ip %s m_recvOffset %d > m_recvBufferSize %d\n", m_addr.getIP().c_str(), m_recvOffset, m_recvBufferSize);
			error = SOCKET_ERROR;
		}
		else
		{
			decodeRecvBuffer();
		}
	}
	else if (error == 0)
	{
		error = SOCKET_ERROR;
	}
	else
	{
		int err = WSAGetLastError();
#ifdef LIBANSWER_WINDOWS
		if (err == WSAEWOULDBLOCK)
		{
			error = 0;
		}
#endif
#ifdef LIBANSWER_LINUX
		if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
		{
			error = 0;
		}
#endif
	}

	return error;
}

int TcpConnection::onSend()
{
	if (m_sendLen == 0)
	{
		encodeSendBuffer();
		
		if (m_sendLen == 0)
		{
			return 0;
		}
	}

	int error = ::send(m_fd, m_sendBuffer+m_sendCursor, m_sendLen-m_sendCursor, 0);
	if (error > 0)
	{
		m_sendCursor += error;
		if (m_sendCursor == m_sendLen)
		{
			m_sendLen = 0;
			m_sendCursor = 0;

			if (m_sendList.size() <= 0)
			{
				disableMask(MASK_WRITE);
			}
		}
		else if (m_sendCursor > m_sendLen)
		{
			LOG_INFO("TcpConnection from ip %s m_sendCursor %d > m_sendLen %d\n", m_addr.getIP().c_str(), m_sendCursor, m_sendLen);
			error = SOCKET_ERROR;
		}
		else
		{
			error = 0;
		}
	}
	else if (error == 0)
	{
		error = SOCKET_ERROR;
	}
	else
	{
		int err = WSAGetLastError();
#ifdef LIBANSWER_WINDOWS
		if (err == WSAEWOULDBLOCK)
		{
			error = 0;
		}
#endif
#ifdef LIBANSWER_LINUX
		if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
		{
			error = 0;
		}
#endif
	}

	return error;
}

bool TcpConnection::canNextSend()
{
	return m_sendLen == 0 && m_sendList.size() > 0;
}

void TcpConnection::sendPacket(NetPacket *packet, bool needEncodeHead)
{
	if (packet != NULL)
	{
		if (!m_bCheckHalt || m_sendList.size() < 5000 || m_sendListByteLen < 100000)
		{
			if (needEncodeHead)
			{
				packet->encodeHeadInfo();
			}
			m_sendList.push(packet);
			m_sendListByteLen += packet->getSize();

			enableMask(MASK_WRITE);
		}
		else
		{
			if (!m_halt)
			{
				int halt = Answer::halt(m_fd);
				m_halt = true;
				LOG_INFO("TcpConnection %p from ip %s halt with packet proc %d in mSendPool.size() = %d, m_sendListByteLen = %d , return %d\n", this, m_addr.getIP().c_str(), static_cast<int>(packet->getProc()), m_sendList.size(), m_sendListByteLen, halt);
			}
			packet->destroy();
		}
	}
}

void* TcpConnection::getPtr()
{
	return m_ptr;
}

void TcpConnection::setPtr(void *ptr)
{
	m_ptr = ptr;
}

void TcpConnection::halt()
{
	if (!m_halt)
	{
		Answer::halt(m_fd);
		m_halt = true;
	}
}

void TcpConnection::decodeRecvBuffer()
{
	int decodeOffset = 0;

	if (m_bCheckPolicy)
	{
#ifdef TENCENT_TGW
		if (m_recvOffset >= POLICY_PACKET_LEN)
		{
			m_bCheckPolicy = false;
			decodeOffset = POLICY_PACKET_LEN;
		}
#else
		if (m_recvOffset >= 23)
		{
			if (strncasecmp("<policy-file-request/>", m_recvBuffer, 22)==0)
			{
				NetPacket *packet = new NetPacket(1024, NULL, false, true);
				char szPolicy[256] = "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>";
				memcpy(packet->getBuffer()-NET_PACKET_HEAD_LEN, szPolicy, strlen(szPolicy)+1);
				packet->setSize(strlen(szPolicy)+1);
				sendPacket(packet, false);
			}

			m_bCheckPolicy = false;
			decodeOffset = 23;
		}
#endif
	}

	if (!m_bCheckPolicy)
	{
		while ((m_decodeState == DECODE_HEAD && m_recvOffset-decodeOffset >= NET_PACKET_HEAD_LEN) || (m_decodeState == DECODE_DATA && m_recvOffset-decodeOffset >0))
		{
			if (m_decodeState == DECODE_HEAD)
			{
				uint8_t packType = *((uint8_t*)(m_recvBuffer+decodeOffset));
				uint16_t proc = *((uint16_t*)(m_recvBuffer+decodeOffset+1));
				uint32_t datalen = *((uint32_t*)(m_recvBuffer+decodeOffset+3));

				if (m_bCheckPackSize && datalen > m_maxPackSize)
				{
					LOG_INFO("TcpConnection::decodeRecvBuffer wrong indata from ip %s, packType %d, proc %d, datalen %d\n", m_addr.getIP().c_str(), (uint32_t)packType, (uint32_t)proc, (uint32_t)datalen);
					Answer::halt(m_fd);
					return;
				}

				decodeOffset += NET_PACKET_HEAD_LEN;

				m_packet = m_processor->popPacket(datalen);
				m_packet->setType(static_cast<PackType>(packType));
				m_packet->setProc(proc);
				m_packet->setSize(datalen);

				m_decodeState = DECODE_DATA;
			}

			if (m_decodeState == DECODE_DATA && m_packet != NULL)
			{
				uint32_t needlen = m_packet->getSize() - m_packet->getWOffset();
				uint32_t haslen = m_recvOffset - decodeOffset;
				uint32_t writelen = (std::min)(needlen, haslen);
				m_packet->write(m_recvBuffer+decodeOffset, writelen);
				decodeOffset += writelen;
				if (m_packet->getSize() == m_packet->getWOffset())
				{
					m_processor->postMessage(TCP_MSG_NETPACKET, this, m_packet);
					m_packet = NULL;
					m_decodeState = DECODE_HEAD;
				}
			}
		}
	}

	if (decodeOffset > 0 && decodeOffset < m_recvOffset)
	{
		memmove(m_recvBuffer, m_recvBuffer+decodeOffset, m_recvOffset-decodeOffset);
	}

	m_recvOffset = m_recvOffset - decodeOffset;
}

void TcpConnection::encodeSendBuffer()
{
	bool bPolicy = false;

	NetPacket *nextPacket = m_sendList.front();
	while (nextPacket != NULL)
	{
		int len = (std::min)(static_cast<int>(nextPacket->getSize())-m_parseLen, m_sendBufferSize-4-m_sendLen);
		if (len <= 0)
		{
			LOG_ERROR("TcpConnection::encodeSendBuffer len = %d, nextPacket->getSize() = %d, m_parseLen = %d, m_sendBufferSize = %d, m_sendLen = %d\n", len, nextPacket->getSize(), m_parseLen, m_sendBufferSize, m_sendLen);
			return;
		}

		memcpy(m_sendBuffer+m_sendLen, nextPacket->getBuffer()-NET_PACKET_HEAD_LEN+m_parseLen, len);
		if (nextPacket->isPolicy())
		{
			bPolicy = true;
		}

		m_parseLen += len;
		if (m_parseLen == static_cast<int>(nextPacket->getSize()))
		{
			m_sendListByteLen -= nextPacket->getSize();
			m_sendList.pop()->destroy();
			nextPacket = m_sendList.front();
			m_parseLen = 0;
		}

		m_sendLen += len;
		if (m_sendLen == m_sendBufferSize-4)
		{
			break;
		}
	}

	if (!bPolicy)
	{
		if (m_bCompress && m_sendLen > 0)
		{
			if (m_sendLen <= 300)
			{
				memmove(m_sendBuffer+4, m_sendBuffer, m_sendLen);
				m_sendLen += 4;
				*((int16_t*)(m_sendBuffer)) = m_sendLen;
				*((int16_t*)(m_sendBuffer+2)) = 0;
			}
			else
			{
				char szCompress[CLIENT_SEND_BUFFER-4] = {};
				int compressLen = Zip::compress(szCompress, CLIENT_SEND_BUFFER-4, m_sendBuffer, m_sendLen);
				if (compressLen > 0 && compressLen < CLIENT_SEND_BUFFER-4)
				{
					memcpy(m_sendBuffer+4, szCompress, compressLen);				
					*((int16_t*)(m_sendBuffer)) = compressLen+4;
					*((int16_t*)(m_sendBuffer+2)) = m_sendLen;
					m_sendLen = compressLen + 4;
				}
				else
				{
					memmove(m_sendBuffer+4, m_sendBuffer, m_sendLen);
					m_sendLen += 4;
					*((int16_t*)(m_sendBuffer)) = m_sendLen;
					*((int16_t*)(m_sendBuffer+2)) = 0;
				}
			}
		}
	}
}

#ifdef LIBANSWER_LINUX
void TcpConnection::setEpfd(int epfd)
{
	m_epfd = epfd;
}
#endif
