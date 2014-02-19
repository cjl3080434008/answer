#include "stdafx.h"

#include "Logger.h"
#include "SockOpts.h"
#include "TcpClient.h"

using namespace Answer;
using namespace std;

TcpClient::TcpClient(int recvBufferSize, int sendBufferSize, unsigned int bufferSize, unsigned int initCount, unsigned int maxCount, std::string name)
	: m_fd(INVALID_SOCKET), m_name(name), m_connected(false), m_recvBufferSize(recvBufferSize), m_packet(NULL), m_sendBufferSize(sendBufferSize), m_bufferSize(bufferSize), m_bufferPool(bufferSize, initCount, maxCount)
{
	m_recvBuffer = new char[m_recvBufferSize];
	m_sendBuffer = new char[m_sendBufferSize];

	reset();
}

TcpClient::~TcpClient()
{
	delete[] m_recvBuffer;
	delete[] m_sendBuffer;
}

void TcpClient::onDisconnect()
{

}

socket_t TcpClient::getFd()
{
	return m_fd;
}

string TcpClient::getName()
{
	return m_name;
}

void TcpClient::reset()
{
	if (m_fd != INVALID_SOCKET)
	{
		Answer::close(m_fd);
		m_fd = INVALID_SOCKET;
	}

	m_connected = false;

	bzero(m_recvBuffer, sizeof(char)*m_recvBufferSize);
	m_recvOffset = 0;

	if (m_packet != NULL)
	{
		m_packet->destroy();
	}
	m_decodeState = DECODE_HEAD;

	NetPacket *packet = NULL;
	while ((packet = m_sendList.pop()) != NULL)
	{
		packet->destroy();
	}
	m_parseLen = 0;

	bzero(m_sendBuffer, sizeof(char)*m_sendBufferSize);
	m_sendLen = 0;
	m_sendCursor = 0;
}

bool TcpClient::connect(const InetAddress &serverAddr)
{
	if (m_fd != INVALID_SOCKET || m_connected)
	{
		LOG_ERROR("TcpClient::connect with m_fd %d, m_connected %d return false\n", m_fd, (int)m_connected);
		return false;
	}

	m_serverAddr = serverAddr;

	m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_fd == INVALID_SOCKET)
	{
		LOG_ERROR("TcpClient::connect socket return INVALID_SOCKET, return false\n");
		return false;
	}

	int err = ::connect(m_fd, sockaddr_cast(&(m_serverAddr.getSockAddr())), m_serverAddr.getAddrLen());
	if (err	== SOCKET_ERROR)
	{
		LOG_ERROR("TcpClient::connect connect return SOCKET_ERROR, errno %d\n", WSAGetLastError());
		Answer::close(m_fd);
		m_fd = INVALID_SOCKET;
		return false;
	}

	Answer::setnonblock(m_fd);
	Answer::setnodelay(m_fd);
	Answer::setkeepalive(m_fd, 300);

	m_connected = true;

	return true;
}

NetPacket* TcpClient::popNetpacket(Answer::PackType packType, uint16_t proc)
{
	return popNetpacket(packType, proc, m_bufferSize);
}

NetPacket* TcpClient::popNetpacket(Answer::PackType packType, uint16_t proc, unsigned int buffersize)
{
	NetPacket *packet =  m_bufferPool.pop(buffersize);
	if (packet != NULL)
	{
		packet->setType(packType);
		packet->setProc(proc);
	}
	return packet;
}

void TcpClient::sendPacket(NetPacket *packet)
{
	if (packet != NULL)
	{
		if (m_connected)
		{
			packet->encodeHeadInfo();
			m_sendList.push(packet);
		}
		else
		{
			packet->destroy();
		}
	}
}

bool TcpClient::needSend()
{
	return m_sendList.size() > 0 || m_sendCursor < m_sendLen;
}

int TcpClient::onRecv()
{
	int error = ::recv(m_fd, m_recvBuffer+m_recvOffset, m_recvBufferSize-m_recvOffset, 0);
	if (error > 0)
	{
		m_recvOffset += error;
		if (m_recvOffset > m_recvBufferSize)
		{
			LOG_INFO("TcpClient to ip %s m_recvOffset %d > m_recvBufferSize %d\n", m_serverAddr.getIP().c_str(), m_recvOffset, m_recvBufferSize);
			error = SOCKET_ERROR;
		}
		else
		{
			decodeRecvBuffer();
		}
	}
	else if (error == 0)
	{
		LOG_INFO("TcpClient %s onRecv recv return 0\n", m_name.c_str());
		error = SOCKET_ERROR;
	}
	else
	{
		int err = WSAGetLastError();
		LOG_INFO("TcpClient %s onRecv recv return %d, errno = %d\n", m_name.c_str(), error, err);
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

int TcpClient::onSend()
{
	if (m_sendLen == 0)
	{
		encodeSendBuffer();
		if (m_sendLen == 0)
		{
			LOG_ERROR("TcpClient::onSend m_sendLen == 0 after encodeSendBuffer\n");
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
		}
		else if (m_sendCursor > m_sendLen)
		{
			LOG_INFO("TcpClient to ip %s m_sendCursor %d > m_sendLen %d\n", m_serverAddr.getIP().c_str(), m_sendCursor, m_sendLen);
			error = SOCKET_ERROR;
		}
	}
	else if (error == 0)
	{
		LOG_INFO("TcpClient %s onSend send return 0\n", m_name.c_str());
		error = SOCKET_ERROR;
	}
	else
	{
		int err = WSAGetLastError();
		LOG_INFO("TcpClient %s onSend send return %d, errno = %d\n", m_name.c_str(), error, err);
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

void TcpClient::decodeRecvBuffer()
{
	int decodeOffset = 0;

	while ((m_decodeState == DECODE_HEAD && m_recvOffset-decodeOffset >= NET_PACKET_HEAD_LEN) || (m_decodeState == DECODE_DATA && m_recvOffset-decodeOffset >0))
	{
		if (m_decodeState == DECODE_HEAD)
		{
			uint8_t packType = *((uint8_t*)(m_recvBuffer+decodeOffset));
			uint16_t proc = *((uint16_t*)(m_recvBuffer+decodeOffset+1));
			uint32_t datalen = *((uint32_t*)(m_recvBuffer+decodeOffset+3));

			if (static_cast<int>(datalen) > m_recvBufferSize)
			{
				LOG_INFO("TcpClient::decodeRecvBuffer very large packet packType = %d, proc = %d, datalen = %d, m_recvBufferSize = %d\n", static_cast<int>(packType), static_cast<int>(proc), datalen, m_recvBufferSize);
			}

			decodeOffset += NET_PACKET_HEAD_LEN;

			m_packet = m_bufferPool.pop(datalen);
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
				onNetPacket(m_packet);
				m_packet = NULL;
				m_decodeState = DECODE_HEAD;
			}
		}
	}

	if (decodeOffset < m_recvOffset)
	{
		memmove(m_recvBuffer, m_recvBuffer+decodeOffset, m_recvOffset-decodeOffset);
	}

	m_recvOffset = m_recvOffset - decodeOffset;
}

void TcpClient::encodeSendBuffer()
{
	NetPacket *nextPacket = m_sendList.front();
	while (nextPacket != NULL)
	{
		int len = (std::min)(static_cast<int>(nextPacket->getSize())-m_parseLen, m_sendBufferSize-m_sendLen);
		if (len <= 0)
		{
			LOG_ERROR("TcpClient::encodeSendBuffer len = %d, nextPacket->getSize() = %d, m_parseLen = %d, m_sendBufferSize = %d, m_sendLen = %d\n", len, nextPacket->getSize(), m_parseLen, m_sendBufferSize, m_sendLen);
			return;
		}

		memcpy(m_sendBuffer+m_sendLen, nextPacket->getBuffer()-NET_PACKET_HEAD_LEN+m_parseLen, len);

		m_parseLen += len;
		if (m_parseLen == static_cast<int>(nextPacket->getSize()))
		{
			m_sendList.pop()->destroy();
			nextPacket = m_sendList.front();
			m_parseLen = 0;
		}

		m_sendLen += len;
		if (m_sendLen == m_sendBufferSize)
		{
			break;
		}
	}
}
