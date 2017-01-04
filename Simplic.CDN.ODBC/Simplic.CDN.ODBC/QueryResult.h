#pragma once
#include "json/json.h"
#include "Info.h"
#include <string>
#include <vector>

/* Contains metadata for a single column of the JSON result set.*/
class ColumnDescriptor
{
private:
	std::string m_name;
	int16_t m_type;
	size_t m_size;

	// InfoRecord that is used when the ODBC application requests information about this object.
	// The InfoRecord object will be initialized on the first request from the application.
	InfoRecord m_info;
	bool m_infoInitialized;

	void initInfoRecord();

public:

	ColumnDescriptor();
	ColumnDescriptor(const Json::Value & jsonColumn);

	void fromJson(const Json::Value & jsonColumn);

	inline int16_t getType() const { return m_type; } 
	inline std::string getName() const { return m_name; }
	inline size_t getSize() const { return m_size; }

	// used by SQLColAttribute to retrieve fields
	bool odbcGetField(
		SQLUSMALLINT    FieldIdentifier,
		SQLPOINTER      CharacterAttributePtr,
		SQLSMALLINT     BufferLength,
		SQLSMALLINT *   StringLengthPtr,
		SQLLEN *        NumericAttributePtr);
};


class QueryResult
{
private:
	// meta-information about the columns in the result set
	std::vector<ColumnDescriptor> m_columnMeta;

	// total amount of rows available to fetch. This is NOT equal to m_rows.size()
	uint32_t m_rowCount;

	// number of affected rows for UPDATE/INSERT/DELETE statements
	int64_t m_affectedRowCount;

	// currently fetched rows
	std::vector<Json::Value*> m_rows;

public:
	inline std::vector<Json::Value*> & rows() { return m_rows; }
	ColumnDescriptor* column(uint32_t i);
	bool fromJson(const Json::Value& apiResponse);

	inline int64_t getAffectedRowCount() { return m_affectedRowCount; }
};