#pragma once

#include <map>
#include <string>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL Inifile
	{
	public:
		Inifile();
		~Inifile();

	private:
		DISALLOW_COPY_AND_ASSIGN(Inifile);

	public:
		bool parse(const std::string &file);
		std::string getStrValue(const std::string &section, const std::string &key);
		int getIntValue(const std::string &section, const std::string &key);

	private:
		void analyze(const std::string &strline);

	private:
		typedef std::map<std::string, std::string> StringMap;
		StringMap m_strMap;
		std::string m_currentSection;
	};
}

