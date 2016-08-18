#include "stdafx.h"
#include "Environment.h"
#include "DbConnection.h"
#include "curl/curl.h"

Environment::Environment()
{
	curl_global_init(CURL_GLOBAL_ALL);
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
