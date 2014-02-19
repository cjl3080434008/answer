#pragma once

#include "Logger.h"

namespace Answer
{
	namespace Application
	{
		ANSWER_EXPORT_SYMBOL void setWorkingDirectory();
		ANSWER_EXPORT_SYMBOL void initLogger(const char *prefix, LogLevel leve);
		ANSWER_EXPORT_SYMBOL int getPid();
		ANSWER_EXPORT_SYMBOL int mkDir(const char *dir);
		ANSWER_EXPORT_SYMBOL void hookSignal();
		ANSWER_EXPORT_SYMBOL void waitSignal();
		ANSWER_EXPORT_SYMBOL void unhookSignal();
		ANSWER_EXPORT_SYMBOL void onSignal(int s);
	};
}

