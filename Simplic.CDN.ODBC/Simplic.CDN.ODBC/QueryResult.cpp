#include "stdafx.h"
#include "QueryResult.h"
#include "OdbcTypeConverter.h"

#include <sqltypes.h>
#include <sql.h>

ColumnDescriptor::ColumnDescriptor()
	:m_name(""),
	m_type(SQL_UNKNOWN_TYPE)
{}

ColumnDescriptor::ColumnDescriptor(const Json::Value & jsonColumn)
{
	fromJson(jsonColumn);
}

void ColumnDescriptor::fromJson(const Json::Value & jsonColumn)
{
	/* example column: { "Name" : "name" , "Type" : 12 } */
	m_name = jsonColumn["Name"].asString();
	m_type = jsonColumn["Type"].asUInt();
	Json::Value jsonSize = jsonColumn["Size"];
	if (jsonSize.isNull())
	{
		m_size = 0;
	}

	if (m_size == 0) // if "Size" was 0 or not set, use the default size
	{
		m_size = OdbcTypeConverter::getInstance()->getColumnSizeByType(m_type);
	}
}


/* ***********************************
              QueryResult
   *********************************** */

ColumnDescriptor* QueryResult::column(uint32_t i)
{
	--i; // ODBC column numbers start from 1, column 0 is a special "bookmark column"
	if (i >= m_columnMeta.size()) return NULL;
	else return &m_columnMeta[i];
}

bool QueryResult::fromJson(const Json::Value& apiResponse)
{
	/* example result:
	{
		"Meta" : {
			"RowCount" : 2
			"Columns" : [
				{ "Name" : "name" , "Type" : 12 }
			]
		},

		"Rows" : [
			["firsttable"],
			["secondtable"]
		]
	} */

	m_columnMeta.clear();

	const Json::Value& meta = apiResponse["Meta"];
	if (meta.isNull()) return false;

	// parse general meta info
	m_rowCount = meta["RowCount"].asUInt();

	// parse column meta info
	const Json::Value& columns = meta["Columns"]; 
	if (columns.isNull()) return false;
	for (auto it = columns.begin(); it != columns.end(); ++it)
	{
		m_columnMeta.push_back(ColumnDescriptor(*it));
	}

	return true;
}