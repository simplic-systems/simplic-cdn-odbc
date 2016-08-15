#pragma once
#include "json/json.h"
#include <string>
#include <vector>

/* Contains metadata for a single column of the JSON result set.*/
class ColumnDescriptor
{
private:
	std::string m_name;
	uint16_t m_type;

public:

	ColumnDescriptor();
	ColumnDescriptor(const Json::Value & jsonColumn);

	void fromJson(const Json::Value & jsonColumn);

	inline uint16_t getType() { return m_type; } 
	inline std::string getName() { return m_name; }

};


class QueryResult
{
private:
	// meta-information about the columns in the result set
	std::vector<ColumnDescriptor> m_columnMeta;

	// total amount of rows available to fetch. This is NOT equal to m_rows.size()
	uint32_t m_rowCount;

	// currently fetched rows
	std::vector<Json::Value*> m_rows;

public:
	inline std::vector<Json::Value*> & rows() { return m_rows; }
	ColumnDescriptor* column(uint32_t i);
	bool fromJson(const Json::Value& apiResponse);


};