#pragma once

#include <set>
#include <mutex>

class DbConnection;

class Environment
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

