#include "stdafx.h"

#include "Database.h"
#include "DBPool.h"

using namespace Answer;

DBPool::DBPool()
{

}

DBPool::~DBPool()
{

}

int DBPool::init(int count, const std::string &host, const std::string &user, const std::string &passwd, const std::string &db, unsigned int port)
{
	if (count <= 0)
	{
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		MySqlDB *connection = new MySqlDB;
		int err = connection->open(host, user, passwd, db, port);
		if (err == 0)
		{
			m_pool.push(connection);
		}
		else
		{
			delete connection;
		}
	}

	return m_pool.size();
}

void DBPool::clear()
{
	while (m_pool.size() > 0)
	{
		MySqlDB *connection = m_pool.pop();
		if (connection != NULL)
		{
			connection->close();
			delete connection;
		}
	}
}

MySqlDB* DBPool::pop()
{
	return m_pool.pop();
}

void DBPool::push(MySqlDB *connection)
{
	if (connection != NULL)
	{
		m_pool.push(connection);
	}
}

MySqlDBGuard::MySqlDBGuard(DBPool &dbpool)
	: m_dbpool(dbpool), m_dbconn(dbpool.pop())
{

}

MySqlDBGuard::~MySqlDBGuard()
{
	if (m_dbconn != NULL)
	{
		m_dbpool.push(m_dbconn);
		m_dbconn = NULL;
	}
}

MySqlQuery& MySqlDBGuard::query(const char *sql)
{
	if (m_dbconn != NULL)
	{
		return m_dbconn->query(sql);
	}
	else
	{
		return m_query;
	}
}

int MySqlDBGuard::excute(const char *sql)
{
	if (m_dbconn != NULL)
	{
		return m_dbconn->excute(sql);
	}
	else
	{
		return ERR_ANSWER_DATABASE;
	}
}

int MySqlDBGuard::lastInsertId()
{
	if (m_dbconn != NULL)
	{
		return m_dbconn->lastInsertId();
	}
	else
	{
		return 0;
	}
}
