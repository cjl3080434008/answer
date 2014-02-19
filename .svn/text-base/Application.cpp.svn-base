#include "stdafx.h"

#include "Application.h"
#include "DayTime.h"
#include "Mutex.h"

#include <signal.h>
#include <string.h>

#ifdef LIBANSWER_WINDOWS
#include <direct.h>
#include <process.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#define SIG_LIBANSWER SIGBREAK
#endif

#ifdef LIBANSWER_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define SIG_LIBANSWER SIGRTMIN+1
#endif

using namespace Answer;
using namespace std;

static Mutex g_mutex;

void Application::setWorkingDirectory()
{
#ifdef LIBANSWER_WINDOWS
	char path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, path, MAX_PATH);
	PathRemoveFileSpecA(path);
	SetCurrentDirectoryA(path);
#endif
}

void Application::initLogger(const char *prefix, LogLevel leve)
{
	char szFile[MAX_PATH] = {};
	mkDir("log");
	tm time = DayTime::localnow();
	snprintf(szFile, sizeof(szFile)-1, "log/%s.%d.%d.%d.%d.%d.%d.%d.log", prefix, time.tm_year+1900, time.tm_mon+1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, getPid());
	Logger::open(szFile, "a");
	Logger::setLogLevel(leve);
}

int Application::getPid()
{
#ifdef LIBANSWER_WINDOWS
	return _getpid();
#endif

#ifdef LIBANSWER_LINUX
	return static_cast<int>(getpid());
#endif
}

int Application::mkDir(const char *dir)
{
#ifdef LIBANSWER_WINDOWS
	return _mkdir(dir);
#endif

#ifdef LIBANSWER_LINUX
	return mkdir(dir, 0777);
#endif
}

void Application::hookSignal()
{
	LOG_INFO("Application::hookSignal %d\n", SIG_LIBANSWER);
	signal(SIG_LIBANSWER, onSignal);
}

void Application::waitSignal()
{
	g_mutex.lock();
	g_mutex.lock();
}

void Application::unhookSignal()
{
	LOG_INFO("Application::unhookSignal %d\n", SIG_LIBANSWER);
	signal(SIG_LIBANSWER, 0);
}

void Application::onSignal(int s)
{
	LOG_INFO("Application::onSignal %d\n", s);
	if (s == SIG_LIBANSWER)
	{
		g_mutex.unlock();
	}
}

