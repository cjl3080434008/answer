#pragma once

#include "SafeQueue.h"

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL NetPacketPool
	{
	public:
		NetPacketPool(unsigned int bufferSize, unsigned int initCount, unsigned int maxCount);
		~NetPacketPool();

	public:
		NetPacket* pop();
		NetPacket* pop(uint32_t size);
		void push(NetPacket *netpacket);

	private:
		NetPacketList m_pool;
		unsigned int m_bufferSize;
		unsigned int m_initCount;
		unsigned int m_maxCount;
	};
}

