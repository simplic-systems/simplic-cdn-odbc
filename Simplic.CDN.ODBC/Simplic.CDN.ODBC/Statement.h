#pragma once

#include "DbConnection.h"
#include "QueryResult.h"
#include "ApplicationDataDescriptor.h"
#include "json/json.h"
#include "odbc_api.h"

#include <set>
#include <mutex>
#include <string>

class Statement
{
private:
	std::recursive_mutex m_mutex;
	DbConnection* m_connection; // parent object
	
	// current part of the result set presented to the ODBC application.
	QueryResult m_currentResult;
	uint32_t m_cursorPos;

	// current SQL query for this statement
	std::string m_query;

	// current result set from the last API call
	Json::Value m_apiResult;

	// ARD / APD data structures containing information about bound
	// columns and parameters respectively.
	ApplicationDataDescriptor m_implicitRowDescriptor;
	ApplicationDataDescriptor m_implicitParameterDescriptor;

	// Active ARD / APD instances. These point to the implicit descriptors above
	// unless the application explicitly specified another descriptor.
	ApplicationDataDescriptor* m_activeRowDescriptor;
	ApplicationDataDescriptor* m_activeParameterDescriptor;


public:
	Statement(DbConnection* conn);
	~Statement();

	// ##### getters / setters #####
	ColumnDescriptor* getColumnDescriptor(uint32_t i);

	// Get number of rows in the currently fetched part of the result set.
	uint32_t getNumFetchedRows();

	// Set the query string (for prepared statements or directly executed queries)
	void setQuery(const std::string& query);

	// ##### buffer binding #####
	bool bindColumn(
		uint16_t columnNumber,
		int16_t targetType,
		SQLLEN bufferLength,
		void* targetPointer,
		SQLLEN* indicatorPointer);

	// ##### data retrieval #####
	SQLRETURN getData(SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr);
	SQLRETURN getColumns(std::string catalogName, std::string schemaName, std::string tableName, std::string columnName);

	// ##### command execution functions #####
	bool getTables(std::string catalogName, std::string schemaName, std::string tableName, std::string tableType);
	bool execute();

	bool fetchNext();
	bool fetch(uint32_t fromRow, uint32_t numRows);
};

