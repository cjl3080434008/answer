#pragma once

#include "BlockingQueue.h"

#include <string>

namespace Answer
{
	class MySqlDB;
	class MySqlQuery;
	
	class ANSWER_EXPORT_SYMBOL DBPool
	{
	public:
		DBPool();
		~DBPool();

	public:
		int init(int count, const std::string &host, const std::string &user, const std::string &passwd, const std::string &db, unsigned int port);
		void clear();

		MySqlDB* pop();
		void push(MySqlDB *connection);

	private:
		BlockingQueue<MySqlDB*> m_pool;
	};

	class ANSWER_EXPORT_SYMBOL MySqlDBGuard
	{
	public:
		MySqlDBGuard(DBPool &dbpool);
		~MySqlDBGuard();

	private:
		DISALLOW_COPY_AND_ASSIGN(MySqlDBGuard);

	public:
		MySqlQuery& query(const char *sql);
		int excute(const char *sql);
		int lastInsertId();

	private:
		DBPool &m_dbpool;
		MySqlDB *m_dbconn;
		MySqlQuery m_query;
	};
}

