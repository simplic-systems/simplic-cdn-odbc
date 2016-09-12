#include "stdafx.h"
#include "Statement.h"
#include "OdbcTypeConverter.h"

#include <sql.h>
#include <sqltypes.h>

Statement::Statement(DbConnection* conn)
{

	m_connection = conn;
}


Statement::~Statement()
{
	m_connection->removeStatement(this);
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


SQLRETURN Statement::getData(SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr)
{
	const Json::Value & row = *m_currentResult.rows()[0];
	--Col_or_Param_Num; // In ODBC, columns are counted starting from 1 => remove that offset
	if (Col_or_Param_Num >= row.size()) return SQL_ERROR;

	const Json::Value & value = row[Col_or_Param_Num];
	return OdbcTypeConverter::getInstance()->jsonToOdbc(&value, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPtr);

}


bool Statement::getTables(std::string catalogName, std::string schemaName, std::string tableName, std::string tableType)
{
	m_cursorPos = 0;

	Json::Value parameters;
	parameters["catalog"] = catalogName;
	parameters["schema"] = schemaName;
	parameters["table"] = tableName;
	parameters["tableType"] = tableType;

	if(!m_connection->executeCommand(m_apiResult, "gettables", parameters))
	{
		return false;
	}

	return m_currentResult.fromJson(m_apiResult);
}

bool Statement::execute()
{
	return false;
}


SQLRETURN Statement::getColumns(std::string catalogName, std::string schemaName, std::string tableName, std::string columnName)
{
	m_cursorPos = 0;

	Json::Value parameters;
	parameters["catalog"] = catalogName;
	parameters["schema"] = schemaName;
	parameters["table"] = tableName;
	parameters["columnname"] = columnName;

	if (m_connection->executeCommand(m_apiResult, "getcolumns", parameters))
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
	//return m_connection->fetch(m_currentResult.rows(), fromRow, numRows);

	m_currentResult.rows().clear();
	Json::Value& rows = m_apiResult["Rows"];
	if (rows.isNull()) return false;

	uint32_t rowsAvailable = rows.size();

	for (uint32_t i = fromRow; i < fromRow + numRows && i < rowsAvailable; ++i)
	{
		m_currentResult.rows().push_back(&rows[i]);
	}

	return true;
}

// parse results and meta-information from json.
// store the meta-information explicitly in data structures,
// keep the result set as a json object.