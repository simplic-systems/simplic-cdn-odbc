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

	// Handle from Simplic.CDN for this statement.
	std::string m_handle;

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

	// Information about the current pending binary download
	bool m_isDownloadPending;
	SQLUSMALLINT m_downloadColumn;

	// Information about the current parameter that needs
	// to be uploaded / is being uploaded
	bool m_isUploadPending;
	bool m_isFirstParamUpload;
	SQLUSMALLINT m_currentParameter;
	ApplicationDataDescriptorItem m_currentParameterDescriptor;
	Json::Value m_jsonParameters;
	Json::Value m_currentJsonParameter;
	std::string m_uploadHandle;

	SQLRETURN downloadBinaryChunk(
		SQLUSMALLINT nColumn,
		SQLPOINTER buffer,
		SQLLEN bufferSize,
		SQLLEN* strLenIndicator);

	SQLRETURN processParametersAndExecute();
	SQLRETURN finishExecution();

	void abortBinaryTransfer();
	void resetParameterUploadState();


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

	bool bindParameter(
		uint16_t columnNumber,
		int16_t targetType,
		SQLLEN bufferLength,
		void* targetPointer,
		SQLLEN* indicatorPointer);

	// ##### data retrieval #####
	SQLRETURN getData(SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr);
	SQLRETURN getColumns(std::string catalogName, std::string schemaName, std::string tableName, std::string columnName);

	// ##### parameter data sending functions #####
	/// equivalent to SQLParamData ODBC function
	SQLRETURN paramData(SQLPOINTER* paramInfo);
	bool putData(SQLPOINTER data, SQLLEN lengthOrIndicator);

	// ##### command execution functions #####
	bool getTables(std::string catalogName, std::string schemaName, std::string tableName, std::string tableType);
	SQLRETURN execute();



	bool fetchNext();
	bool fetch(uint32_t fromRow, uint32_t numRows);
};

