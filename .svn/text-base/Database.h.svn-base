#pragma once

#include <mysql.h>
#include <string>

namespace Answer
{
	class ANSWER_EXPORT_SYMBOL MySqlQuery
	{
	public:
		MySqlQuery();
		MySqlQuery(MySqlQuery &rQuery);

		~MySqlQuery();

		MySqlQuery& operator=(MySqlQuery &rQuery);

	public:
		void setRes(MYSQL_RES *res);

		int getFieldCount();
		int getRowCount();

		int seekRow(int index);
		void nextRow();

		bool eof();

		const char* getStringValue(int fieldIndex, const char *strNull="");
		const char* getStringValue(const char *fieldName, const char *strNull="");

		int getIntValue(int fieldIndex, const int intNull=0);
		int getIntValue(const char *fieldName, const int intNull=0);

		long long getLongLongValue(int fieldIndex, const long long longlongNull=0);
		long long getLongLongValue(const char *fieldName, const long long longlongNull=0);

		int getFieldIndex(const char *fieldName);
		const char* getFieldName(int fieldIndex, const char *strNull="");

	private:
		void freeRes();

	public:
		MYSQL_RES *m_result;
		int m_fieldCount;
		int m_rowCount;
		MYSQL_ROW m_currentRow;
	};

	class ANSWER_EXPORT_SYMBOL MySqlDB
	{
	public:
		MySqlDB();
		~MySqlDB();

	public:
		int open(const std::string &host, const std::string &user, const std::string &passwd, const std::string &db, unsigned int port);
		void close();
		int reopen();
		
		MySqlQuery& query(const char *sql);
		int excute(const char *sql);
		int lastInsertId();

	private:
		MYSQL *m_connection;
		std::string m_host;
		std::string m_user;
		std::string m_passwd;
		std::string m_db;
		unsigned int m_port;

		MySqlQuery m_query;
	};
}

