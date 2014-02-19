#pragma once

#include <string>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL InetAddress
	{
	public:
		InetAddress();
		InetAddress(uint16_t port);
		InetAddress(const std::string &ip, uint16_t port);
		InetAddress(uint32_t ip, uint16_t port);
		InetAddress(const sockaddr_in &addr);
		~InetAddress();

	public:
		const sockaddr_in& getSockAddr();
		socklen_t getAddrLen();
		std::string getIP();
		uint16_t getPort();

	private:
		sockaddr_in m_addr;
	};
}

