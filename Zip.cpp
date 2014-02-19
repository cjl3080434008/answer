#include "stdafx.h"

#include "Logger.h"
#include "Zip.h"

#include <stdio.h>
#include <stdlib.h>

using namespace Answer;

int32_t Zip::uncompressFile(const char *filename, uint8_t *dest, uint32_t *destLen)
{
	if (filename == NULL || dest == NULL || destLen == NULL)
	{
		LOG_ERROR("Zlib::uncompressFile with filename = %p, dest = %p, destLen = %p\n", filename, dest, destLen);
		return ERR_ANSWER_FALSE;
	}

	FILE *file = fopen(filename, "rb");
	if (file == NULL)
	{
		LOG_ERROR("Zlib::uncompressFile %s fopen return NULL, errno = %d\n", filename, errno);
		return ERR_ANSWER_FALSE;
	}

	fseek(file, 0, SEEK_END);
	long filelen = ftell(file);
	fseek(file, 0, SEEK_SET);

	Bytef *pSrc = (Bytef*)calloc(filelen, sizeof(Bytef));

	size_t srcLen = fread(pSrc, sizeof(Bytef), filelen, file);

	uLongf uDestLen = (uLongf)(*destLen);

	int zerr = uncompress(dest, &uDestLen, pSrc, srcLen);
	if (zerr != Z_OK)
	{
		LOG_ERROR("Zlib::uncompressFile uncompress return %d\n", zerr);
		*destLen = (uint32_t)uDestLen;
		free(pSrc);
		return ERR_ANSWER_FALSE;
	}

	*destLen = (uint32_t)uDestLen;
	free(pSrc);
	
	return ERR_ANSWER_OK;
}

int32_t Zip::compress(char *dest, unsigned long destlen, const char *src, unsigned long srclen)
{
	int zerr = compress2((Bytef*)dest, &destlen, (const Bytef*)src, srclen, 9);
	if (zerr != Z_OK)
	{
		LOG_ERROR("Zip::compress return %d\n", zerr);
		return -1;
	}

	return destlen;
}

