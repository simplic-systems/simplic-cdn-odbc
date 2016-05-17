#include "stdafx.h"
#include "Environment.h"
#include "DbConnection.h"

Environment::Environment()
{

}

Environment::~Environment()
{
	LOCK(m_mutex);
	for (DbConnection* conn: m_connections) delete conn;
	m_connections.clear();
}

DbConnection * Environment::createConnection()
{
	LOCK(m_mutex);
	DbConnection* conn = new DbConnection(this);
	m_connections.insert(conn);
	return conn;
}

void Environment::removeConnection(DbConnection * conn)
{
	LOCK(m_mutex);
	m_connections.erase(conn);
}
