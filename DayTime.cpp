#include "stdafx.h"

#include "DayTime.h"

#ifdef LIBANSWER_LINUX
#include <sys/time.h>
#endif

using namespace Answer;

int64_t DayTime::tick()
{
#ifdef LIBANSWER_WINDOWS
	return GetTickCount();
#endif

#ifdef LIBANSWER_LINUX
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000+tv.tv_usec/1000;
#endif
}

int32_t DayTime::now()
{
	return static_cast<int32_t>(time(NULL));
}

tm DayTime::localnow()
{
	time_t nowtime = time(NULL);

	tm result = {};
	localtime_r(&nowtime, &result);

	return result;
}

void DayTime::sleep(int milliseconds)
{
#ifdef LIBANSWER_WINDOWS
	Sleep(milliseconds);
#endif

#ifdef LIBANSWER_LINUX
	timespec req;
	req.tv_nsec = milliseconds%1000;
	req.tv_sec = (milliseconds-req.tv_nsec)/1000;
	req.tv_nsec = req.tv_nsec*1000000;
	timespec rem;
	nanosleep(&req, &rem);
#endif
}

int DayTime::daydiff(int nowtime, int oldtime)
{
	int oldzero = dayzero(oldtime);
	int nowzero = dayzero(nowtime);

	return (nowzero - oldzero)/86400;
}

int DayTime::dayzero(int time)
{
	time_t ttime = (time_t)time;
	tm tmtime = {};
	localtime_r(&ttime, &tmtime);

	return (int)(ttime - tmtime.tm_sec - tmtime.tm_min*60 - tmtime.tm_hour*3600);
}

int32_t DayTime::hourzero(int time)
{
	time_t ttime = (time_t)time;
	tm tmtime = {};
	localtime_r(&ttime, &tmtime);

	return (int)(ttime - tmtime.tm_sec - tmtime.tm_min*60);
}

int DayTime::weekdiff(int nowtime, int oldtime)
{
	time_t told = (time_t)oldtime;
	tm tmold = {};
	localtime_r(&told, &tmold);
	tmold.tm_wday = (tmold.tm_wday + 6)%7;

	time_t tnow = (time_t)nowtime;
	tm tmnow = {};
	localtime_r(&tnow, &tmnow);
	tmnow.tm_wday = (tmnow.tm_wday + 6)%7;

	int oldzero = (int)(told - tmold.tm_sec - tmold.tm_min*60 - tmold.tm_hour*3600 - tmold.tm_wday*86400);
	int nowzero = (int)(tnow - tmnow.tm_sec - tmnow.tm_min*60 - tmnow.tm_hour*3600 - tmnow.tm_wday*86400);

	return (nowzero - oldzero)/604800;
}

int DayTime::monthdiff(int nowtime, int oldtime)
{
	time_t told = (time_t)oldtime;
	tm tmold = {};
	localtime_r(&told, &tmold);

	time_t tnow = (time_t)nowtime;
	tm tmnow = {};
	localtime_r(&tnow, &tmnow);

	return tmnow.tm_mon - tmold.tm_mon;
}

bool DayTime::betweendate(tm nowlocal, int32_t begin_date, int32_t end_date)
{
	int32_t date = (nowlocal.tm_year+1900)*10000 + (nowlocal.tm_mon+1)*100 + nowlocal.tm_mday;

	return date >= begin_date && date <= end_date;
}

int DayTime::adddate(int32_t begin_date, int32_t addon)
{
	tm begintm = {};
	begintm.tm_year = begin_date/10000 - 1900;
	begintm.tm_mon = (begin_date%10000)/100 - 1;
	begintm.tm_mday = begin_date%100;

	time_t endtime = mktime(&begintm) + addon * 86400;
	tm endtm = {};
	localtime_r(&endtime, &endtm);

	return (endtm.tm_year+1900)*10000 + (endtm.tm_mon+1)*100 + endtm.tm_mday;
}

int DayTime::getdate(int32_t time)
{
	time_t ttime = (time_t)time;
	tm tmtime = {};
	localtime_r(&ttime, &tmtime);

	return (tmtime.tm_year+1900)*10000 + (tmtime.tm_mon+1)*100 + tmtime.tm_mday;
}
