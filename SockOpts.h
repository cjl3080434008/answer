#pragma once

namespace Answer
{
	const sockaddr* sockaddr_cast(const sockaddr_in *addr);
	sockaddr* sockaddr_cast(sockaddr_in *addr);

	int setkeepalive(socket_t fd, int idleseconds);

	int setnodelay(socket_t fd);
	int setnonblock(socket_t fd);
	int setreuseaddr(socket_t fd);
	int setreuseport(socket_t fd);

	int getreadbuffer(socket_t fd);
	int setreadbuffer(socket_t fd, socket_t size);
	int getwritebuffer(socket_t fd);
	int setwritebuffer(socket_t fd, socket_t size);

	int close(socket_t fd);
	int halt(socket_t fd);
}

