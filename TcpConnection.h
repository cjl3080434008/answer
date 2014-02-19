#pragma once

#include "InetAddress.h"
#include "SafeQueue.h"

#include <string>

namespace Answer
{
	class NetPacket;
	class TcpProcessor;

	class ANSWER_EXPORT_SYMBOL TcpConnection
	{
	public:
		TcpConnection();
		~TcpConnection();

	public:
		void reset();
		void bind(socket_t fd, const InetAddress &addr, TcpProcessor *processor);

		socket_t getFd();
		std::string getIP();

		void setCheckPackSize(bool check);
		void setCheckHalt(bool check);
		void setCheckPolicy(bool check);
		void setCompress(bool compress);

		int getMask();
		void enableMask(MultiplexorMask mask);
		void disableMask(MultiplexorMask mask);

		void onConnect();
		void onDisconnect();

		int onRecv();
		int onSend();
		bool canNextSend();

		void sendPacket(NetPacket *packet, bool needEncodeHead);

		void* getPtr();
		void setPtr(void *ptr);

		void halt();

	private:
		void decodeRecvBuffer();
		void encodeSendBuffer();

	private:
		socket_t m_fd;
		bool m_halt;

		InetAddress m_addr;
		TcpProcessor *m_processor;

		char m_defaultRecvBuffer[CLIENT_RECV_BUFFER];
		char m_defaultSendBuffer[CLIENT_SEND_BUFFER];

		int m_recvBufferSize;
		char *m_recvBuffer;
		int m_recvOffset;
		NetPacket *m_packet;
		int m_decodeState;

		bool m_bCheckPackSize;
		bool m_bCheckHalt;
		bool m_bCheckPolicy;
		bool m_bCompress;

		uint32_t m_maxPackSize;

		NetPacketList m_sendList;
		int m_sendListByteLen;

		int m_parseLen;
		int m_sendBufferSize;
		char *m_sendBuffer;
		int m_sendLen;
		int m_sendCursor;

		int m_mask;

		void *m_ptr;

#ifdef LIBANSWER_LINUX
	public:
		void setEpfd(int epfd);

	private:
		int m_epfd;
#endif
	};
}

