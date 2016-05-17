#pragma once

#include "Environment.h"

#include <set>
#include <mutex>

class Statement;

class DbConnection
{
private:
	std::recursive_mutex m_mutex;

	Environment* m_environment;
	std::set<Statement*> m_statements;
public:
	DbConnection(Environment* env);
	~DbConnection();

	Statement* createStatement();
	void removeStatement(Statement* stmt);
};

