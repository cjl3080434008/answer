#include "stdafx.h"

#include "InetAddress.h"

using namespace Answer;

InetAddress::InetAddress()
{
	bzero(&m_addr, sizeof(m_addr));
}

InetAddress::InetAddress(uint16_t port)
{
	bzero(&m_addr, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
	bzero(&m_addr, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(uint32_t ip, uint16_t port)
{
	bzero(&m_addr, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = port;
	m_addr.sin_addr.s_addr = ip;
}

InetAddress::InetAddress(const sockaddr_in &addr)
: m_addr(addr)
{

}

InetAddress::~InetAddress()
{

}

const sockaddr_in& InetAddress::getSockAddr()
{
	return m_addr;
}

socklen_t InetAddress::getAddrLen()
{
	return sizeof(m_addr);
}

std::string InetAddress::getIP()
{
	return inet_ntoa(m_addr.sin_addr);
}

uint16_t InetAddress::getPort()
{
	return ntohs(m_addr.sin_port);
}

