#include "stdafx.h"
#include "QueryResult.h"
#include "OdbcTypeConverter.h"
#include "odbc_api.h"

#include <sqlext.h>


ColumnDescriptor::ColumnDescriptor()
	:m_name(""),
	m_type(SQL_UNKNOWN_TYPE),
	m_infoInitialized(false)
{}

ColumnDescriptor::ColumnDescriptor(const Json::Value & jsonColumn)
	: m_infoInitialized(false)
{
	fromJson(jsonColumn);
}

void ColumnDescriptor::initInfoRecord()
{
	std::function<std::string()> fnName = [this]() {return getName(); } ;
	std::function<int16_t()> fnType = [this]() {return getType(); };
	std::function<uint64_t()> fnSize = [this]() {return getSize(); };

	m_info.addField(SQL_DESC_BASE_COLUMN_NAME, new StringInfoField(fnName));
	m_info.addField(SQL_DESC_NAME, new StringInfoField(fnName));
	m_info.addField(SQL_DESC_TYPE, new FixedSizeInfoField<int16_t>(fnType));
	m_info.addField(SQL_DESC_CONCISE_TYPE, new FixedSizeInfoField<int16_t>(fnType));
	m_info.addField(SQL_DESC_LENGTH, new FixedSizeInfoField<uint64_t>(fnSize));

	// ---- dummy attributes ----
	// autoincrement = false
	m_info.addField(SQL_DESC_AUTO_UNIQUE_VALUE, new FixedSizeInfoField<uint64_t>(0));

	m_infoInitialized = true;
}

void ColumnDescriptor::fromJson(const Json::Value & jsonColumn)
{
	/* example column: { "Name" : "name" , "Type" : 12 } */
	m_name = jsonColumn["Name"].asString();
	m_type = jsonColumn["Type"].asInt();
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


bool ColumnDescriptor::odbcGetField(
	SQLUSMALLINT    FieldIdentifier,
	SQLPOINTER      CharacterAttributePtr,
	SQLSMALLINT     BufferLength,
	SQLSMALLINT *   StringLengthPtr,
	SQLLEN *        NumericAttributePtr)
{
	if (!m_infoInitialized) initInfoRecord();
	OdbcInfoField* field = m_info.getField(FieldIdentifier);
	if (field == NULL)
	{
		return false;
	}
	else return field->toOdbcSegregated(
		CharacterAttributePtr,
		NumericAttributePtr,
		BufferLength,
		StringLengthPtr
	);
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