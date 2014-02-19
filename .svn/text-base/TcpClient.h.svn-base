#pragma once

#include "InetAddress.h"
#include "NetPacket.h"
#include "NetPacketPool.h"
#include "SafeQueue.h"

#include <string>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL TcpClient
	{
	public:
		TcpClient(int recvBufferSize, int sendBufferSize, unsigned int bufferSize, unsigned int initCount, unsigned int maxCount, std::string name);
		virtual ~TcpClient();

	public:
		virtual void onDisconnect();

	protected:
		virtual void onNetPacket(Answer::NetPacket *inPacket) = 0;

	public:
		socket_t getFd();
		std::string getName();

		void reset();
		bool connect(const Answer::InetAddress &serverAddr);

		Answer::NetPacket* popNetpacket(Answer::PackType packType, uint16_t proc);
		Answer::NetPacket* popNetpacket(Answer::PackType packType, uint16_t proc, unsigned int buffersize);
		void sendPacket(Answer::NetPacket *packet);

		bool needSend();

		int onRecv();
		int onSend();

		void decodeRecvBuffer();
		void encodeSendBuffer();

	protected:
		socket_t m_fd;

		InetAddress m_serverAddr;
		std::string m_name;

		bool m_connected;

		int m_recvBufferSize;
		char *m_recvBuffer;
		int m_recvOffset;
		NetPacket *m_packet;
		int m_decodeState;

		NetPacketList m_sendList;
		int m_parseLen;
		int m_sendBufferSize;
		char *m_sendBuffer;
		int m_sendLen;
		int m_sendCursor;

		unsigned int m_bufferSize;
		NetPacketPool m_bufferPool;
	};
}


