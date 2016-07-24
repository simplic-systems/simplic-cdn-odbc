#pragma once

#include "Environment.h"

#include <stdint.h>
#include <set>
#include <string>
#include <mutex>

class Statement;

class DbConnection
{
private:
	std::recursive_mutex m_mutex;

	Environment* m_environment;
	std::set<Statement*> m_statements;

	uint32_t m_timeout;
public:
	DbConnection(Environment* env);
	~DbConnection();

	Statement* createStatement();
	void removeStatement(Statement* stmt);

	void setTimeout(uint32_t timeout);

	bool connect(std::string url, std::string user, std::string password);

};

