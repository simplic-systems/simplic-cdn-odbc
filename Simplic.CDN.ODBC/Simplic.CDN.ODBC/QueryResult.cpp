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
	/* example column: { "name" : "name" , "type" : 12 } */
	m_name = jsonColumn["name"].asString();
	m_type = jsonColumn["type"].asUInt();
	Json::Value jsonSize = jsonColumn["size"];
	if (jsonSize.isNull())
	{
		m_size = OdbcTypeConverter::getInstance()->getColumnSizeByType(m_type);
	}
	else
	{
		m_size = jsonSize.asUInt64();
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
		"meta" : {
			"rowcount" : 2
			"columns" : [
				{ "name" : "name" , "type" : 12 }
			]
		},

		"rows" : [
			["firsttable"],
			["secondtable"]
		]
	} */

	m_columnMeta.clear();

	const Json::Value& meta = apiResponse["meta"];
	if (meta.isNull()) return false;

	// parse general meta info
	m_rowCount = meta["rowcount"].asUInt();

	// parse column meta info
	const Json::Value& columns = meta["columns"]; 
	if (columns.isNull()) return false;
	for (auto it = columns.begin(); it != columns.end(); ++it)
	{
		m_columnMeta.push_back(ColumnDescriptor(*it));
	}

	return true;
}