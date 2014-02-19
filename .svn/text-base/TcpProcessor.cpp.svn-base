#include "stdafx.h"

#include "Logger.h"
#include "NetPacket.h"
#include "TcpAcceptor.h"
#include "TcpProcessor.h"
#include "TcpServer.h"

using namespace Answer;

TcpProcessor::TcpProcessor(TcpAcceptor *acceptor, TcpServer *server, unsigned int packSize, unsigned int initPacketCount, unsigned int maxPacketCount)
: m_acceptor(acceptor), m_server(server), m_netpacketPool(packSize, initPacketCount, maxPacketCount), m_packSize(packSize)
{

}

TcpProcessor::~TcpProcessor()
{

}

void TcpProcessor::run()
{
	while (true)
	{
		TcpMessage msg = m_messageQueue.pop();

		switch (msg.type)
		{
		case TCP_MSG_CONNECT: onConnect(msg.conn); break;
		case TCP_MSG_DISCONNECT: onDisconnect(msg.conn);	break;
		case TCP_MSG_NETPACKET: onNetpacket(msg.conn, msg.packet); break;
		case TCP_MSG_STOP: return; //like WM_QUIT...
		default: LOG_ERROR("TcpProcessor::run with unknown msgtype:%d\n", msg.type);	break;
		}
	}
}

void TcpProcessor::stop()
{
	postMessage(TCP_MSG_STOP, NULL, NULL);
	join();
}

void TcpProcessor::postMessage(TcpMessageType type, TcpConnection *conn, NetPacket *packet)
{
	TcpMessage msg;
	msg.type = type;
	msg.conn = conn;
	msg.packet = packet;
	m_messageQueue.push(msg);
}

NetPacket* TcpProcessor::popPacket()
{
	return m_netpacketPool.pop(m_packSize);
}

NetPacket* TcpProcessor::popPacket(uint32_t size)
{
	return m_netpacketPool.pop(size);
}

uint32_t TcpProcessor::getPacketSize()
{
	return m_packSize;
}

void TcpProcessor::onConnect(TcpConnection *conn)
{
	if (m_server != NULL)
	{
		m_server->onConnect(conn);
	}
}

void TcpProcessor::onDisconnect(TcpConnection *conn)
{
	if (m_server != NULL)
	{
		m_server->onDisconnect(conn);
	}

	if (m_acceptor != NULL)
	{
		m_acceptor->pushConn(conn);
	}
}

void TcpProcessor::onNetpacket(TcpConnection *conn, NetPacket *inPacket)
{
	if (inPacket == NULL)
	{
		return;
	}

	if (conn == NULL || m_server == NULL)
	{
		inPacket->destroy();
	}

	m_server->onNetPacket(conn, inPacket);

	inPacket->destroy();
}

