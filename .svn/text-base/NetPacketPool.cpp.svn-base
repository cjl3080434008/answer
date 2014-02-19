#include "stdafx.h"

#include "NetPacket.h"
#include "NetPacketPool.h"

using namespace Answer;

NetPacketPool::NetPacketPool(unsigned int bufferSize, unsigned int initCount, unsigned int maxCount)
	: m_bufferSize(bufferSize), m_initCount(initCount), m_maxCount(maxCount)
{
	for (unsigned int i = 0; i < m_initCount; ++i)
	{
		NetPacket *netpacket = new NetPacket(bufferSize+NET_PACKET_HEAD_LEN, this);
		m_pool.push(netpacket);
	}
}

NetPacketPool::~NetPacketPool()
{
	NetPacket *packet = NULL;
	while ((packet=m_pool.pop()) != NULL)
	{
		delete packet;
	}
}

NetPacket* NetPacketPool::pop()
{
	NetPacket *netpacket = m_pool.pop();
	if (netpacket == NULL)
	{
		netpacket = new NetPacket(m_bufferSize+NET_PACKET_HEAD_LEN, this);
	}

	netpacket->reset();
	
	return netpacket;
}

NetPacket* NetPacketPool::pop(uint32_t size)
{
	if (size <= m_bufferSize)
	{
		return pop();
	}
	else
	{
		NetPacket *netpacket = new NetPacket(size+NET_PACKET_HEAD_LEN, this);
		netpacket->reset();
		return netpacket;
	}
}

void NetPacketPool::push(NetPacket *netpacket)
{
	if (netpacket != NULL && netpacket->subRefCount() == 0)
	{
		if (netpacket->getBufferSize() > m_bufferSize)
		{
			delete netpacket;
		}
		else if (m_pool.size() >= static_cast<int>(m_maxCount))
		{
			delete netpacket;
		}
		else
		{
			m_pool.push(netpacket);
		}
	}
}

