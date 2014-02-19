#include "stdafx.h"

#include "StringUtility.h"

#include <string.h>

using namespace Answer;

int StringUtility::utf8Strlen(const char *str)
{
	if (str == NULL)
	{
		return 0;
	}

	int byteLength = static_cast<int>(strlen(str));

	int length = 0;

	int index = 0;
	while (index < byteLength)
	{
		char ch = str[index];

		length++;

		index++;

		if (index >= byteLength)
		{
			break;
		}

		if (ch & 0x80)
		{
			ch <<= 1;
			while(ch & 0x80)
			{
				index++;
				ch <<=1;
			}
		}
	}

	return length;
}

StringVector StringUtility::split(const std::string& str, const std::string& delims /* =  */, uint32_t maxSplits /* = 0 */)
{
	StringVector ret;  

	if (!str.empty())
	{
		uint32_t numSplits = 0;  

		size_t start = 0;
		size_t pos = 0;  
		do   
		{  
			pos = str.find_first_of(delims, start);  
			if (pos == start)  
			{  
				start = pos + 1;  
			}  
			else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))  
			{  
				ret.push_back( str.substr(start) );  
				break;  
			}  
			else  
			{  
				ret.push_back( str.substr(start, pos - start) );  
				start = pos + 1;  
			}  

			start = str.find_first_not_of(delims, start);  
			++numSplits;  

		} while (pos != std::string::npos);  
	}

	return ret;  
}

