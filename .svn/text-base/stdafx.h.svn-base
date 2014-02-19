#pragma once

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef LIBANSWER_WINDOWS
#include <WinSock2.h>
#include <Windows.h>
#include <mstcpip.h>
#pragma comment(lib, "Ws2_32.lib")

typedef SOCKET socket_t;
typedef int socklen_t;

#ifdef LIBANSWER_EXPORTS
#define ANSWER_EXPORT_SYMBOL __declspec(dllexport)
#else
#define ANSWER_EXPORT_SYMBOL __declspec(dllimport)
#endif

#define bzero ZeroMemory
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf _snprintf
#define atoll _atoi64
#define AtomicAdd(a) InterlockedIncrement(a)
#define AtomicSub(a) InterlockedDecrement(a)
#define AnswerAllocator std::allocator
#endif

#ifdef LIBANSWER_LINUX
#include <arpa/inet.h>
#include <errno.h>
#include <ext/pool_allocator.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

typedef int socket_t;
#define MAX_PATH 260
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define WSAGetLastError() errno
#define WSAEWOULDBLOCK EAGAIN

#define ANSWER_EXPORT_SYMBOL
#define AtomicAdd(a) __sync_add_and_fetch(a, 1)
#define AtomicSub(a) __sync_sub_and_fetch(a, 1)
#define AnswerAllocator __gnu_cxx::__pool_alloc
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Answer
{
#define NET_PACKET_HEAD_LEN 7

#define POLICY_PACKET_LEN 65

#define CLIENT_RECV_BUFFER 8192
#define CLIENT_SEND_BUFFER 8192

	enum ErrorCode
	{
		ERR_ANSWER_OK,
		ERR_ANSWER_FALSE,
		ERR_ANSWER_OVERFLOW,
		ERR_ANSWER_SOCKET,
		ERR_ANSWER_DATABASE,
	};

	enum MultiplexorMask
	{
		MASK_NONE = 0x0000,
		MASK_READ = 0x0001,
		MASK_WRITE = 0x0002,
	};

	enum DecodeState
	{
		DECODE_HEAD,
		DECODE_DATA,
	};

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	TypeName& operator=(const TypeName&)
}



