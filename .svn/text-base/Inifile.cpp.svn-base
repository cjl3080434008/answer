#include "stdafx.h"

#include "Inifile.h"

#include <algorithm>
#include <fstream>
#include <vector>

using namespace Answer;
using namespace std;

static const string MIDDLESTRING = "::";

Inifile::Inifile()
{

}

Inifile::~Inifile()
{

}

bool Inifile::parse(const std::string &file)
{
	ifstream fin(file.c_str());
	if(!fin.is_open())
	{
		return false;
	}

	while(!fin.eof())
	{
		string strline;
		getline(fin, strline, '\n');
		analyze(strline);
	}

	return !m_strMap.empty(); 
}

std::string Inifile::getStrValue(const std::string &section, const std::string &key)
{
	string ret;

	StringMap::iterator it = m_strMap.find(section + MIDDLESTRING + key);
	if(it != m_strMap.end())
	{
		ret = it->second;
	}

	return ret;
}

int Inifile::getIntValue(const std::string &section, const std::string &key)
{
	return atoi(getStrValue(section, key).c_str());
}

void Inifile::analyze(const std::string &strline)
{
	string::size_type first =strline.find('[');
	string::size_type last = strline.rfind(']');
	if( first != string::npos && last != string::npos && first != last+1)
	{
		m_currentSection = strline.substr(first+1, last-first-1);
		return;
	}

	if((first=strline.find('='))== string::npos)
	{
		return;
	}

	string strhead= strline.substr(0, first);
	string strtail=strline.substr(first+1, string::npos);
	first= strhead.find_first_not_of(" \t");
	last = strhead.find_last_not_of(" \t");
	if(first == string::npos || last == string::npos)
	{
		return;
	}

	string strkey = strhead.substr(first, last-first+1);
	first = strtail.find_first_not_of(" \t");
	if( ((last = strtail.find("\t#", first ))  != string::npos)  || 
		((last = strtail.find(" #", first ))   != string::npos)  ||
		((last = strtail.find("\t//", first )) != string::npos)  ||
		((last = strtail.find(" //", first ))  != string::npos) )
	{
		strtail = strtail.substr(0, last-first);
	}	
	last = strtail.find_last_not_of(" \t");
	if(first == string::npos || last == string::npos)
	{
		return ;
	}

	m_strMap[m_currentSection + MIDDLESTRING + strkey] = strtail.substr(first, last-first+1);
	return ;
}
