#pragma once

#include <stdio.h>

namespace Answer
{
	enum LogLevel
	{
		LOG_LEVEL_ALL,
		LOG_LEVEL_DEBUG,
		LOG_LEVEL_INFO,
		LOG_LEVEL_WARN,
		LOG_LEVEL_ERROR,
		LOG_LEVEL_OFF,
	};

#define MAX_LOG_LENTH 1024

	class ANSWER_EXPORT_SYMBOL Logger
	{
	public:
		Logger();
		~Logger();

	public:
		static int open(const char *path, const char *mode);
		static int close();

		static void print(LogLevel level, char *format, ...);

		static LogLevel getLogLevel();
		static void setLogLevel(LogLevel level);

	private:
		static FILE *m_fp;
		static LogLevel m_logLevel;
		static char* m_logLevelName[LOG_LEVEL_OFF];
	};

#define LOG_DEBUG(...) Answer::Logger::print(Answer::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  Answer::Logger::print(Answer::LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOG_WARN(...)  Answer::Logger::print(Answer::LOG_LEVEL_WARN,  __VA_ARGS__)
#define LOG_ERROR(...) Answer::Logger::print(Answer::LOG_LEVEL_ERROR, __VA_ARGS__)
}

