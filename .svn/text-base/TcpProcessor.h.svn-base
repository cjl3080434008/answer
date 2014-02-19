#pragma once

#include "BlockingQueue.h"
#include "NetPacketPool.h"
#include "Thread.h"

namespace Answer
{
	enum TcpMessageType
	{
		TCP_MSG_UNKNOWN,
		TCP_MSG_CONNECT,
		TCP_MSG_DISCONNECT,
		TCP_MSG_NETPACKET,
		TCP_MSG_STOP,
	};

	class TcpConnection;
	class NetPacket;
	struct TcpMessage 
	{
		explicit TcpMessage(int param = 0) //just for BlockingQueue
		{
			type = TCP_MSG_UNKNOWN;
			conn = NULL;
			packet = NULL;
		}

		TcpMessageType type;
		TcpConnection *conn;
		NetPacket *packet;
	};

	class TcpAcceptor;
	class TcpServer;
	class TcpProcessor
		: public Thread
	{
	public:
		TcpProcessor(TcpAcceptor *acceptor, TcpServer *server, unsigned int packSize, unsigned int initPacketCount, unsigned int maxPacketCount);
		~TcpProcessor();

	protected:
		virtual void run();

	public:
		void stop();

		void postMessage(TcpMessageType type, TcpConnection *conn, NetPacket *packet);

		NetPacket* popPacket();
		NetPacket* popPacket(unsigned int size);
		uint32_t getPacketSize();

	private:
		void onConnect(TcpConnection *conn);
		void onDisconnect(TcpConnection *conn);
		void onNetpacket(TcpConnection *conn, NetPacket *inPacket);

	private:
		TcpAcceptor *m_acceptor;
		TcpServer *m_server;

		NetPacketPool m_netpacketPool;
		unsigned int m_packSize;

		BlockingQueue<TcpMessage> m_messageQueue;
	};
}

