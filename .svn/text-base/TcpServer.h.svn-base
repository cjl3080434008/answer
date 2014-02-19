#pragma once

#include "NetPacketPool.h"

namespace Answer
{
	class NetPacket;
	class TcpAcceptor;
	class TcpConnection;

	class ANSWER_EXPORT_SYMBOL TcpServer
	{
	public:
		TcpServer(unsigned int netpackSize, unsigned int initNetpacketCount, unsigned int maxNetpacketCount, bool bCompress, bool bCheckPolicy);
		virtual	~TcpServer();

	public:
		int start(int port, int backlog, int iothreadnum, int processthreadnum, unsigned int initConnPoolCount, unsigned int maxConnPoolCount, uint32_t bufferSize, unsigned int initPacketCount, unsigned int maxPacketCount);
		void stop();

		NetPacket* popNetpacket();
		NetPacket* popNetpacket(unsigned int packsize);

		virtual void onConnect(Answer::TcpConnection *conn) = 0;
		virtual void onDisconnect(Answer::TcpConnection *conn) = 0;
		virtual void onNetPacket(Answer::TcpConnection *conn, Answer::NetPacket *inPacket) = 0;

		void replySuccess(Answer::TcpConnection *conn, uint16_t proc, int32_t addon = 0);
		void replyFailure(Answer::TcpConnection *conn, uint16_t proc, int32_t errcode, int32_t addon = 0);

	private:
		unsigned int m_netpackSize;
		NetPacketPool m_netpackPool;
		TcpAcceptor *m_acceptor;
		bool m_bCompress;
		bool m_bCheckPolicy;
	};
}


