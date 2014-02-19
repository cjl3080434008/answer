#pragma once

#include <pthread.h>
#include <time.h>

namespace Answer
{
	namespace DayTime
	{
		ANSWER_EXPORT_SYMBOL int64_t tick();
		ANSWER_EXPORT_SYMBOL int32_t now();
		ANSWER_EXPORT_SYMBOL tm localnow();
		ANSWER_EXPORT_SYMBOL void sleep(int milliseconds);
		ANSWER_EXPORT_SYMBOL int daydiff(int nowtime, int oldtime);
		ANSWER_EXPORT_SYMBOL int dayzero(int time);
		ANSWER_EXPORT_SYMBOL int hourzero(int time);
		ANSWER_EXPORT_SYMBOL int weekdiff(int nowtime, int oldtime);
		ANSWER_EXPORT_SYMBOL int monthdiff(int nowtime, int oldtime);
		ANSWER_EXPORT_SYMBOL bool betweendate(tm nowlocal, int32_t begin_date, int32_t end_date);
		ANSWER_EXPORT_SYMBOL int adddate(int32_t begin_date, int32_t addon);
		ANSWER_EXPORT_SYMBOL int getdate(int32_t time);
	}
}

