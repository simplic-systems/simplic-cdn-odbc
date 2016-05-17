#pragma once

#include "DbConnection.h"

#include <set>
#include <mutex>

class Statement
{
private:
	std::recursive_mutex m_mutex;

	DbConnection* m_connection; // parent object

public:
	Statement(DbConnection* conn);
	~Statement();
};

