#pragma once

#include <map>

#include "Info.h"
#include "QueryResult.h"
#include "json\json-forwards.h"
#include "odbc_api.h"
#include "OdbcApiObject.h"

class ApplicationDataDescriptorItem;

/**
 * The class ApplicationDataDescriptor contains information about how the application wants us to deliver the result data.
 * This includes information on buffers bound by the application to result columns or parameters.
 * It corresponds to the "ARD" and "APD" described in the ODBC doocumentation: https://msdn.microsoft.com/en-us/library/ms716339(v=vs.85).aspx
 */
class ApplicationDataDescriptor : public OdbcApiObject
{
private:
	// general information

	bool m_isExplicitlyAllocated;
	
	// information about each bound column / parameter
	std::map<uint16_t, ApplicationDataDescriptorItem> m_items;
public:
	ApplicationDataDescriptor();
	
	inline bool isExplicitlyAllocated() { return m_isExplicitlyAllocated; }
	bool getItem(size_t nItem, ApplicationDataDescriptorItem& result);

	/** Binds a column/parameter buffer as in SQLBindCol */
	bool bind(
		uint16_t columnOrParamNumber,
		int16_t targetType,
		SQLLEN bufferLength,
		void* targetPointer,
		SQLLEN* indicatorPointer);

	/** populates all bound buffers using the values from the specified rowset */
	SQLRETURN populateBoundBuffersFromResultSet(QueryResult& resultSet);
};



class ApplicationDataDescriptorItem
{
private:
	int16_t m_targetType; // Data type used/requested by the application
	SQLLEN m_bufferLength; // length of the data buffer 
	void* m_targetPointer; // Pointer to the buffer for the column/parameter data
	SQLLEN* m_indicatorPointer; // Pointer to the buffer for string length / indicator information

public:
	ApplicationDataDescriptorItem();
	ApplicationDataDescriptorItem(
		int16_t targetType,
		SQLLEN bufferLength,
		void* targetPointer,
		SQLLEN* indicatorPointer);
	ApplicationDataDescriptorItem(const ApplicationDataDescriptorItem& other);

	/** Converts the specified json value to the type specified by this descriptor item
	 *  and writes the converted value into the bound buffer. */
	SQLRETURN toBoundBuffer(const Json::Value* jsonValue, const ColumnDescriptor* metaData);

	inline bool isBufferBound() const { return m_targetPointer != NULL; }
	inline int16_t getTargetType() const { return m_targetType; }
	inline SQLLEN getBufferLength() const { return m_bufferLength; }
	inline void* getTargetPointer() const { return m_targetPointer; }
	inline SQLLEN* getIndicatorPointer() const { return m_indicatorPointer; }
};