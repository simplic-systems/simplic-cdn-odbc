#include "stdafx.h"
#include "ApplicationDataDescriptor.h"
#include "OdbcTypeConverter.h"

ApplicationDataDescriptor::ApplicationDataDescriptor()
	: m_isExplicitlyAllocated(false)
{ }


bool ApplicationDataDescriptor::bind(uint16_t columnOrParamNumber, int16_t targetType, SQLLEN bufferLength, void * targetPointer, SQLLEN * indicatorPointer)
{
	if (columnOrParamNumber == 0) return false; // bookmark column is not supported yet
	m_items[columnOrParamNumber] = ApplicationDataDescriptorItem(targetType, bufferLength, targetPointer, indicatorPointer);
	return true;
}


SQLRETURN ApplicationDataDescriptor::populateBoundBuffersFromResultSet(QueryResult& resultSet)
{
	bool success = true;

	// iterate over all bound buffers and populate them using data from jsonRows.
	// TODO: Implement support for multi-row buffers
	for (auto it = m_items.begin(); it != m_items.end(); ++it)
	{
		uint16_t columnOrParamNum = it->first;
		const Json::Value& jsonRow = *(resultSet.rows()[0]); // use first row
		const Json::Value& jsonValue = jsonRow[columnOrParamNum - 1];

		if (it->second.isBufferBound())
		{
			SQLRETURN result = it->second.toBoundBuffer(&jsonValue);
			success &= result == SQL_SUCCESS;
		}
	}

	return success ? SQL_SUCCESS : SQL_ERROR; 
}


ApplicationDataDescriptorItem::ApplicationDataDescriptorItem()
	: ApplicationDataDescriptorItem(SQL_UNKNOWN_TYPE, 0, NULL, NULL)
{ }


ApplicationDataDescriptorItem::ApplicationDataDescriptorItem(int16_t targetType, SQLLEN bufferLength, void * targetPointer, SQLLEN * indicatorPointer)
{
	m_targetType = targetType;
	m_bufferLength = bufferLength;
	m_targetPointer = targetPointer;
	m_indicatorPointer = indicatorPointer;
}


SQLRETURN ApplicationDataDescriptorItem::toBoundBuffer(const Json::Value * jsonValue)
{
	return OdbcTypeConverter::getInstance()->jsonToOdbc(
		jsonValue,
		m_targetType,
		m_targetPointer,
		m_bufferLength,
		m_indicatorPointer);
}
