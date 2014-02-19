#pragma once

#ifdef LIBANSWER_WINDOWS
#define ZLIB_WINAPI
#endif

#include "zconf.h"
#include "zlib.h"

namespace Answer
{
	namespace Zip
	{
		ANSWER_EXPORT_SYMBOL int32_t uncompressFile(const char *filename, uint8_t *dest, uint32_t *destLen);
		ANSWER_EXPORT_SYMBOL int32_t compress(char *dest, unsigned long destlen, const char *src, unsigned long srclen);
	}
}

