#include "stdafx.h"
#include "DbConnection.h"
#include "Statement.h"


DbConnection::DbConnection(Environment * env)
	: m_timeout(10)
{
	m_environment = env;
}

DbConnection::~DbConnection()
{
	LOCK(m_mutex);
	for (Statement* stmt : m_statements) delete stmt;
	m_statements.clear();
	m_environment->removeConnection(this);
}

Statement * DbConnection::createStatement()
{
	LOCK(m_mutex);
	Statement* stmt = new Statement(this);
	m_statements.insert(stmt);
	return stmt;
}

void DbConnection::removeStatement(Statement * stmt)
{
	LOCK(m_mutex);
	m_statements.erase(stmt);
}

void DbConnection::setTimeout(uint32_t timeout)
{
	m_timeout = timeout;
}

bool DbConnection::connect(std::string url, std::string user, std::string password)
{
	// dummy
	if (user == std::string("exampleuser")) return true; else return false;
}
