#pragma once

#include "OdbcApiObject.h"
#include <set>
#include <mutex>

class DbConnection;

class Environment : public OdbcApiObject
{
private:
	std::recursive_mutex m_mutex;
	std::set<DbConnection*> m_connections;

public:
	Environment();
	~Environment();

	DbConnection* createConnection();
	void removeConnection(DbConnection* conn);
};

