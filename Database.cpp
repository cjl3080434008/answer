#include "stdafx.h"

#include "Database.h"
#include "Logger.h"

#include <errmsg.h>

using namespace Answer;

MySqlQuery::MySqlQuery()
: m_result(NULL), m_fieldCount(0), m_rowCount(0), m_currentRow(NULL)
{

}

MySqlQuery::MySqlQuery(MySqlQuery &rQuery)
: m_result(NULL), m_fieldCount(0), m_rowCount(0), m_currentRow(NULL)
{
	*this = rQuery;
}

MySqlQuery::~MySqlQuery()
{
	freeRes();
}

MySqlQuery& MySqlQuery::operator=(MySqlQuery &rQuery)
{
	if (this == &rQuery)
	{
		return *this;
	}

	freeRes();

	m_result = rQuery.m_result;
	m_fieldCount = 0;
	m_rowCount = 0;
	m_currentRow = NULL;

	if (m_result != NULL)
	{
		mysql_data_seek(m_result, 0);
		m_fieldCount = mysql_num_fields(m_result);
		m_rowCount = static_cast<int>(mysql_num_rows(m_result));
		m_currentRow = mysql_fetch_row(m_result);
	}

	rQuery.m_result = NULL;
	rQuery.m_fieldCount = 0;
	rQuery.m_rowCount = 0;
	rQuery.m_currentRow = NULL;

	return *this;
}

void MySqlQuery::setRes(MYSQL_RES *res)
{
	this->freeRes();
	m_result = res;
	if (m_result != NULL)
	{
		m_fieldCount = mysql_num_fields(m_result);
		m_rowCount = static_cast<int>(mysql_num_rows(m_result));
		
		mysql_data_seek(m_result, 0);
		m_currentRow = mysql_fetch_row(m_result);
	}
}

void MySqlQuery::freeRes()
{
	if (m_result != NULL)
	{
		mysql_free_result(m_result);
		m_result = NULL;
		m_rowCount = 0;
		m_fieldCount = 0;
		m_currentRow = NULL;
	}
}

int MySqlQuery::getFieldCount()
{
	return m_fieldCount;
}

int MySqlQuery::getRowCount()
{
	return m_rowCount;
}

int MySqlQuery::seekRow(int index)
{
	if (m_rowCount <= 0 || index < 0 || index >= m_rowCount)
	{
		return 0;		
	}

	mysql_data_seek(m_result, index);
	m_currentRow = mysql_fetch_row(m_result);

	return index;
}

void MySqlQuery::nextRow()
{
	if (m_result != NULL)
	{
		m_currentRow = mysql_fetch_row(m_result);
	}
}


bool MySqlQuery::eof()
{
	return m_currentRow == NULL;
}

const char* MySqlQuery::getStringValue(int fieldIndex, const char *strNull/* = */)
{
	if (m_result == NULL || m_currentRow == NULL || m_fieldCount <= 0 || fieldIndex < 0 || fieldIndex >= m_fieldCount)
	{
		return strNull;
	}

	return m_currentRow[fieldIndex];
}

const char* MySqlQuery::getStringValue(const char *fieldName, const char *strNull/* = */)
{
	if (m_result == NULL || m_currentRow == NULL || fieldName == NULL)
	{
		return strNull;
	}

	int fieldIndx = this->getFieldIndex(fieldName);

	return this->getStringValue(fieldIndx, strNull);
}

int MySqlQuery::getIntValue(int fieldIndex, const int intNull/* =0 */)
{
	if (m_result == NULL || m_currentRow == NULL || m_fieldCount <= 0 || fieldIndex < 0 || fieldIndex >= m_fieldCount || m_currentRow[fieldIndex] == NULL)
	{
		return intNull;
	}

	return atoi(m_currentRow[fieldIndex]);
}

int MySqlQuery::getIntValue(const char *fieldName, const int intNull/* =0 */)
{
	if (m_result == NULL || m_currentRow == NULL || fieldName == NULL)
	{
		return intNull;
	}

	int fieldIndx = this->getFieldIndex(fieldName);

	return this->getIntValue(fieldIndx, intNull);
}

long long MySqlQuery::getLongLongValue(int fieldIndex, const long long longlongNull/* =0 */)
{
	if (m_result == NULL || m_currentRow == NULL || m_fieldCount <= 0 || fieldIndex < 0 || fieldIndex >= m_fieldCount || m_currentRow[fieldIndex] == NULL)
	{
		return longlongNull;
	}

	return atoll(m_currentRow[fieldIndex]);
}

long long MySqlQuery::getLongLongValue(const char *fieldName, const long long longlongNull/* =0 */)
{
	if (m_result == NULL || m_currentRow == NULL || fieldName == NULL)
	{
		return longlongNull;
	}

	int fieldIndx = this->getFieldIndex(fieldName);

	return this->getLongLongValue(fieldIndx, longlongNull);
}

int MySqlQuery::getFieldIndex(const char *fieldName)
{
	if (m_result == NULL || m_fieldCount <= 0 || fieldName == NULL)
	{
		return -1;
	}

	mysql_field_seek(m_result, 0);
	for (int i = 0; i < m_fieldCount; ++i)
	{
		MYSQL_FIELD *field = mysql_fetch_field(m_result);
		if (field != NULL && strcasecmp(field->name, fieldName) == 0)
		{
			return i;
		}
	}

	return -1;
}

const char* MySqlQuery::getFieldName(int fieldIndex, const char *strNull/* = */)
{
	if (m_result == NULL || m_fieldCount <= 0)
	{
		return strNull;
	}

	if (fieldIndex < 0 || fieldIndex >= m_fieldCount)
	{
		return strNull;
	}

	mysql_field_seek(m_result, fieldIndex);
	MYSQL_FIELD *field = mysql_fetch_field(m_result);
	if (field != NULL)
	{
		return field->name;
	}
	else
	{
		return strNull;
	}
}

MySqlDB::MySqlDB()
: m_connection(NULL), m_port(0)
{

}

MySqlDB::~MySqlDB()
{
	this->close();
}

int MySqlDB::open(const std::string &host, const std::string &user, const std::string &passwd, const std::string &db, unsigned int port)
{
	MYSQL *pConnection = mysql_init(NULL);
	if (pConnection == NULL)
	{
		LOG_ERROR("mysql_init failed\n");
		return ERR_ANSWER_DATABASE;
	}

	if (mysql_real_connect(pConnection, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, NULL, 0) == NULL)
	{
		LOG_ERROR("mysql_real_connect failed with error %s\n", mysql_error(pConnection));
		return ERR_ANSWER_DATABASE;
	}

	if (mysql_set_character_set(pConnection, "utf8") != 0)
	{
		LOG_ERROR("mysql_set_character_set with error %s\n", mysql_error(pConnection));
		mysql_close(pConnection);
		return ERR_ANSWER_DATABASE;
	}
	 
	this->close();

	m_connection = pConnection;

	m_host = host;
	m_user = user;
	m_passwd = passwd;
	m_db = db;
	m_port = port;

	return 0;
}

void MySqlDB::close()
{
	if (m_connection != NULL)
	{
		mysql_close(m_connection);
		m_connection = NULL;
	}
}

int MySqlDB::reopen()
{
	return this->open(m_host, m_user, m_passwd, m_db, m_port);
}

MySqlQuery& MySqlDB::query(const char *sql)
{
	int error = this->excute(sql);
	if (error == 0)
	{
		m_query.setRes(mysql_store_result(m_connection));
	}

	return m_query;
}

int MySqlDB::excute(const char *sql)
{
	if (m_connection == NULL || sql == NULL)
	{
		LOG_ERROR("MySqlDB::excute return with m_connection = %p, sql = %p\n", m_connection, sql);
		return ERR_ANSWER_DATABASE;
	}

	if (mysql_query(m_connection, sql) != 0)
	{
		int error = mysql_errno(m_connection);
		if (error == CR_COMMANDS_OUT_OF_SYNC || error == CR_SERVER_GONE_ERROR || error == CR_SERVER_LOST)
		{
			LOG_ERROR("mysql_real_query failed with error = %d, sql = %s, try reopen\n", error, sql);
			this->reopen();

			if (mysql_query(m_connection, sql) != 0)
			{
				LOG_ERROR("mysql_real_query still failed after reopen\n");
				return ERR_ANSWER_DATABASE;
			}
			else
			{
				LOG_ERROR("mysql_real_query succeed after reopen\n");
				return ERR_ANSWER_OK;
			}
		}
		else
		{
			LOG_ERROR("mysql_real_query failed with error = %d, sql = %s\n", error, sql);
			return ERR_ANSWER_DATABASE;
		}
	}
		
	return ERR_ANSWER_OK;
}

int MySqlDB::lastInsertId()
{
	return static_cast<int>(mysql_insert_id(m_connection));
}

