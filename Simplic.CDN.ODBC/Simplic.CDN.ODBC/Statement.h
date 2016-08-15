#pragma once

#include "DbConnection.h"
#include "QueryResult.h"
#include "json/json.h"

#include <sqltypes.h>

#include <set>
#include <mutex>
#include <string>

class Statement
{
private:
	std::recursive_mutex m_mutex;
	DbConnection* m_connection; // parent object
	
	QueryResult m_currentResult;
	uint32_t m_cursorPos;

public:
	Statement(DbConnection* conn);
	~Statement();

	// ##### getters / setters #####
	ColumnDescriptor* getColumnDescriptor(uint32_t i);

	// Get number of rows in the currently fetched part of the result set.
	uint32_t getNumFetchedRows();

	// ##### data retrieval #####
	SQLRETURN getData(SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr);

	// ##### command execution functions #####

	bool getTables(std::string catalogName, std::string schemaName, std::string tableName, std::string tableType);

	bool fetchNext();
	bool fetch(uint32_t fromRow, uint32_t numRows);
};

