#include "stdafx.h"

#include "Logger.h"

#include <stdarg.h>
#include <time.h>

using namespace Answer;

FILE* Logger::m_fp = stdout;
LogLevel Logger::m_logLevel = LOG_LEVEL_ALL;
char* Logger::m_logLevelName[LOG_LEVEL_OFF] = {"", "DEBUG", "INFO", "WARN", "ERROR"};

Logger::Logger()
{

}

Logger::~Logger()
{

}

int Logger::open(const char *path, const char *mode)
{
	if (m_fp != NULL && m_fp != stdout && m_fp != stderr)
	{
		fclose(m_fp);
		m_fp = NULL;
	}

	FILE *fp = fopen(path, mode);
	if (fp == NULL)
	{
		return errno;
	}

	m_fp = fp;
	return 0;
}

int Logger::close()
{
	if (m_fp != NULL && m_fp != stdout && m_fp != stderr)
	{
		int err = fclose(m_fp);
		return err;
	}

	return 0;
}

void Logger::print(LogLevel level, char *format, ...)
{
	if (m_fp != NULL && level >= m_logLevel && level < LOG_LEVEL_OFF)
	{
		int timestamp = static_cast<int>(time(NULL));
		char szLog[MAX_LOG_LENTH] = {};
		int offset = snprintf(szLog, sizeof(szLog)-1, "[%10d][%5s] ", timestamp, m_logLevelName[level]);
		if (offset > 0 && offset < static_cast<int>(sizeof(szLog)-1))
		{
			va_list vl;
			va_start(vl, format);
			offset += vsnprintf(szLog+offset, sizeof(szLog)-1-offset, format, vl);
			if (offset > 0)
			{
				fputs(szLog, m_fp);	
				fflush(m_fp);
			}
		}
	}
}

LogLevel Logger::getLogLevel()
{
	return m_logLevel;
}

void Logger::setLogLevel(LogLevel level)
{
	if (level >= LOG_LEVEL_ALL && level < LOG_LEVEL_OFF)
	{
		m_logLevel = level;
	}
}

