#include "stdafx.h"

#include "DayTime.h"
#include "Random.h"

#include <algorithm>
#include <time.h>

using namespace Answer;

Random::Random()
	: m_engine(DayTime::now())
{

}

Random::~Random()
{

}

int Random::generate(int min, int max)
{
	if (min == max)
	{
		return min;
	}

	if (min > max)
	{
		std::swap(min, max);
	}

	return min + static_cast<int>(m_engine.real()*(max+1-min));
}
