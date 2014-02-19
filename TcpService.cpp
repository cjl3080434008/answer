#include "stdafx.h"

#include "SockOpts.h"
#include "TcpService.h"

using namespace Answer;

TcpService::TcpService(int recvBufferSize, int sendBufferSize, unsigned int bufferSize, unsigned int initCount, unsigned int maxCount, std::string name)
	: TcpClient(recvBufferSize, sendBufferSize, bufferSize, initCount, maxCount, name)
{

}

TcpService::~TcpService()
{

}

void TcpService::run()
{
	if (!m_connected)
	{
		LOG_ERROR("TcpClient::run() enter with m_connected = false, return\n");
		return;
	}

	while (m_connected)
	{
		fd_set rfds;
		fd_set wfds;

		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		FD_SET(m_fd, &rfds);
		if (needSend())
		{
			FD_SET(m_fd, &wfds);
		}

		timeval timeout = {};
		timeout.tv_sec = 0;
		timeout.tv_usec = 50*1000;

		int err = select((int)(m_fd+1), &rfds, &wfds, NULL, &timeout);
		if (err > 0)
		{
			if (FD_ISSET(m_fd, &rfds))
			{
				if (onRecv() == SOCKET_ERROR)
				{
					LOG_ERROR("TcpClient %s run onRecv return SOCKET_ERROR\n", m_name.c_str());
					m_connected = false;
				}
			}

			if (FD_ISSET(m_fd, &wfds))
			{
				if (onSend() == SOCKET_ERROR)
				{
					LOG_ERROR("TcpClient %s run onSend return SOCKET_ERROR\n", m_name.c_str());
					m_connected =false;
				}
			}
		}
		else if (err == 0)
		{
			continue;
		}
		else
		{
			int error = WSAGetLastError();
			LOG_ERROR("TcpClient %s run select return %d, errno = %d\n", m_name.c_str(), err, error);
#ifdef LIBANSWER_WINDOWS
			m_connected = false;
#endif
#ifdef LIBANSWER_LINUX
			if (error != EINTR)
			{
				m_connected = false;
			}
#endif
		}
	}

	Answer::close(m_fd);
	m_fd = INVALID_SOCKET;
}

void TcpService::sendPacketTo(int16_t index, Answer::NetPacket *inPacket)
{
	if (inPacket != NULL)
	{
		uint32_t oldSize = inPacket->getSize();
		if (oldSize == 0)
		{
			int32_t proc = inPacket->getProc();
			LOG_ERROR("TcpService::sendPacketTo size == 0 with proc = %d\n", proc);
		}
		uint32_t needSize = sizeof(int16_t)+sizeof(int16_t);
		if (inPacket->rightShift(needSize))
		{
			inPacket->writeInt16(1);
			inPacket->writeInt16(index);
			inPacket->setSize(oldSize+needSize);
			sendPacket(inPacket);
		}
		else
		{
			NetPacket *packet = popNetpacket(inPacket->getType(), inPacket->getProc(), sizeof(int16_t)+sizeof(int16_t)+inPacket->getSize());
			packet->writeInt16(1);
			packet->writeInt16(index);
			packet->write(inPacket->getBuffer(), inPacket->getSize());
			packet->setSize(packet->getWOffset());
			inPacket->destroy();
			sendPacket(packet);
		}
	}
}

void TcpService::replySuccess(int16_t index, uint16_t proc, int32_t addon/* = 0*/)
{
	NetPacket *packet = popNetpacket(PACK_PROCREPLY, proc);
	packet->writeInt16(1);
	packet->writeInt16(index);
	packet->writeUInt8(1);
	packet->writeInt32(addon);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void TcpService::replyfailure(int16_t index, uint16_t proc, int32_t errcode, int32_t addon/* = 0*/)
{
	NetPacket *packet = popNetpacket(PACK_PROCREPLY, proc);
	packet->writeInt16(1);
	packet->writeInt16(index);
	packet->writeUInt8(0);
	packet->writeInt32(errcode);
	packet->writeInt32(addon);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}
