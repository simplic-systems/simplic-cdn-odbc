#include "stdafx.h"
#include "Info.h"

StringInfoField::StringInfoField(std::string value)
	: DynamicInfoField<std::string>(value) {}

StringInfoField::StringInfoField(std::function<std::string()> dynamicValue)
	: DynamicInfoField<std::string>(dynamicValue) {}

bool StringInfoField::fromOdbc(SQLPOINTER buffer, SQLSMALLINT bufferLength)
{
	setValue(std::string((const char*)buffer, (size_t)bufferLength));
	return true;
}

bool StringInfoField::toOdbc(SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT * dataLengthPtr)
{
	std::string value = getValue();
	*dataLengthPtr = (SQLSMALLINT)value.size();
	SQLSMALLINT bytesToCopy = min(SQLSMALLINT(value.size()) + 1, bufferLength);
	memcpy(buffer, value.c_str(), bytesToCopy);
	return bytesToCopy <= bufferLength;
}

bool StringInfoField::toOdbcSegregated(
	SQLPOINTER      charBuffer,
	SQLLEN *        numericBuffer,
	SQLSMALLINT     bufferLength,
	SQLSMALLINT *   dataLengthPtr)
{
	return toOdbc(charBuffer, bufferLength, dataLengthPtr);
}

OdbcInfoField* StringInfoField::clone()
{
	StringInfoField *cloned = new StringInfoField();
	cloned->setValue(m_dynamicValue);
	return cloned;
}



InfoRecord::InfoRecord()
{
}

InfoRecord::~InfoRecord()
{
	for (std::pair<int, OdbcInfoField* > pair : m_fields) delete pair.second;
	m_fields.clear();
}

OdbcInfoField* InfoRecord::getField(int key)
{
	auto it = m_fields.find(key);
	if (it == m_fields.end()) return NULL;
	else return it->second;
}


void InfoRecord::addField(int key, OdbcInfoField* field)
{
	OdbcInfoField* prevField = getField(key);
	if (prevField != NULL) delete prevField;

	m_fields[key] = field;
}