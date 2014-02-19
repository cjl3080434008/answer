#pragma once

#include "TcpClient.h"
#include "Thread.h"

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL TcpService
		: public Answer::TcpClient, public Answer::Thread
	{
	public:
		TcpService(int recvBufferSize, int sendBufferSize, unsigned int bufferSize, unsigned int initCount, unsigned int maxCount, std::string name);
		virtual ~TcpService();

	protected:
		virtual void onNetPacket(Answer::NetPacket *inPacket) = 0;
		virtual void run();

	public:
		void sendPacketTo(int16_t index, Answer::NetPacket *inPacket);
		void replySuccess(int16_t index, uint16_t proc, int32_t addon = 0);
		void replyfailure(int16_t index, uint16_t proc, int32_t errcode, int32_t addon = 0);
	};
}

