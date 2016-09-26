#include "stdafx.h"
#include "Statement.h"
#include "OdbcTypeConverter.h"
#include "odbc_api.h"
#include "Helper.h"

#include <sqlext.h>

Statement::Statement(DbConnection* conn)
{
	m_connection = conn;
	m_activeRowDescriptor = &m_implicitRowDescriptor;
	m_activeParameterDescriptor = &m_implicitParameterDescriptor;
	m_isDownloadPending = false;
	m_downloadColumn = 0;
}


Statement::~Statement()
{
	m_connection->removeStatement(this);
}


SQLRETURN Statement::downloadBinaryChunk(
	SQLUSMALLINT nColumn,
	SQLPOINTER buffer,
	SQLLEN bufferSize,
	SQLLEN* strLenIndicator)
{
	// if we are already downloading something else, abort that download first!
	if (m_isDownloadPending && nColumn != m_downloadColumn)
	{
		abortBinaryTransfer();
	}

	// if we try to get data from a download that has already completed, return SQL_NO_DATA.
	if (!m_isDownloadPending && nColumn == m_downloadColumn)
	{
		*strLenIndicator = 0;
		return SQL_NO_DATA;
	}

	// if we are starting a new download, we need to set it up first.
	if (!m_isDownloadPending)
	{
		const Json::Value & value = (*m_currentResult.rows()[0])[nColumn - 1];
		bool ok = m_connection->beginDownload(value.asString());
		if (!ok) return SQL_ERROR;

		m_isDownloadPending = true;
		m_downloadColumn = nColumn;
	}

	// download the next (or first) chunk
	bool isCompleted = false;
	int64_t bytesRead = m_connection->downloadChunk(buffer, bufferSize, &isCompleted);

	if (bytesRead < 0)
	{
		abortBinaryTransfer();
		return SQL_ERROR;
	}

	if (isCompleted) m_isDownloadPending = false;


	/* TODO: Return SQL_SUCCESS_WITH_INFO with SQLSTATE 01004 and diag message "Binary data, right truncated"
	*       unless this was the last chunk of the download */
	if(strLenIndicator) *strLenIndicator = isCompleted ? bytesRead : SQL_NO_TOTAL;
	return SQL_SUCCESS;
}

void Statement::abortBinaryTransfer()
{
	m_isDownloadPending = false;
	m_downloadColumn = 0;
	m_connection->resetTransfer();
}

ColumnDescriptor* Statement::getColumnDescriptor(uint32_t i)
{
	return m_currentResult.column(i);
}

uint32_t Statement::getNumFetchedRows()
{
	return (uint32_t) m_currentResult.rows().size();
}

void Statement::setQuery(const std::string& query)
{
	m_query = query;
}

bool Statement::bindColumn(uint16_t columnNumber, int16_t targetType, SQLLEN bufferLength, void * targetPointer, SQLLEN * indicatorPointer)
{
	return m_activeRowDescriptor->bind(
		columnNumber,
		targetType,
		bufferLength,
		targetPointer,
		indicatorPointer);
}


SQLRETURN Statement::getData(SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr)
{
	const Json::Value & row = *m_currentResult.rows()[0];
	if (Col_or_Param_Num > row.size()) return SQL_ERROR;

	const Json::Value & value = row[Col_or_Param_Num-1]; // subtract 1 to account for the missing "bookmark column"
	ColumnDescriptor* meta = m_currentResult.column(Col_or_Param_Num);

	if (Helper::isBinaryType(meta->getType()) && Helper::isBinaryType(TargetType))
		return downloadBinaryChunk(Col_or_Param_Num, TargetValuePtr, BufferLength, StrLen_or_IndPtr);

	return OdbcTypeConverter::getInstance()->jsonToOdbc(&value, meta, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPtr);

}


bool Statement::getTables(std::string catalogName, std::string schemaName, std::string tableName, std::string tableType)
{
	m_cursorPos = 0;

	Json::Value parameters;
	parameters["catalog"] = catalogName;
	parameters["schema"] = schemaName;
	parameters["table"] = tableName;
	parameters["tableType"] = tableType;

	if(!m_connection->executeGetCommand(m_apiResult, "gettables", parameters))
	{
		return false;
	}

	return m_currentResult.fromJson(m_apiResult);
}


SQLRETURN Statement::getColumns(std::string catalogName, std::string schemaName, std::string tableName, std::string columnName)
{
	m_cursorPos = 0;

	Json::Value parameters;
	parameters["catalog"] = catalogName;
	parameters["schema"] = schemaName;
	parameters["table"] = tableName;
	parameters["columnname"] = columnName;

	if (!m_connection->executeGetCommand(m_apiResult, "getcolumns", parameters))
	{
		return false;
	}

	return m_currentResult.fromJson(m_apiResult);
}


bool Statement::execute()
{
	m_cursorPos = 0;

	Json::Value parameters;
	parameters["query"] = m_query;

	// TODO: Pass all bound parameters
	parameters["parameters"] = Json::Value(Json::arrayValue); 

	if (!m_connection->executePostCommand(m_apiResult, "execute", parameters))
	{
		return false;
	}

	return m_currentResult.fromJson(m_apiResult);
}

bool Statement::fetchNext()
{
	bool result = fetch(m_cursorPos, 1);
	++m_cursorPos;
	return result;
}

bool Statement::fetch(uint32_t fromRow, uint32_t numRows)
{
	// any download that's currently active becomes invalid when fetching new rows.
	abortBinaryTransfer();

	m_currentResult.rows().clear();
	Json::Value& rows = m_apiResult["Rows"];
	if (rows.isNull()) return false;

	uint32_t rowsAvailable = rows.size() - fromRow;

	// write data to the internal result set
	for (uint32_t i = fromRow; i < fromRow + min(numRows, rowsAvailable); ++i)
	{
		m_currentResult.rows().push_back(&rows[i]);
	}

	// write data to the bound buffers
	if (rowsAvailable > 0)
	{
		m_activeRowDescriptor->populateBoundBuffersFromResultSet(m_currentResult);
	}

	return true;
}