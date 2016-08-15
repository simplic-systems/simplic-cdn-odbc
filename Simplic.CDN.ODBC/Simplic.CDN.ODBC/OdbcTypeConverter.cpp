#include "stdafx.h"
#include "OdbcTypeConverter.h"
#include "Helper.h"
#include <sql.h>

OdbcTypeConverter* OdbcTypeConverter::_singletonInstance;

SQLRETURN convertToOdbcVarchar(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);


OdbcTypeConverter::OdbcTypeConverter()
{
	// add all conversion functions we want to use 
	m_toOdbcConverters[SQL_VARCHAR] = JsonToOdbcConverter(convertToOdbcVarchar);
	m_toOdbcConverters[SQL_CHAR] = JsonToOdbcConverter(convertToOdbcVarchar);

}


void OdbcTypeConverter::createSingletonInstance()
{
	_singletonInstance = new OdbcTypeConverter();
}

void OdbcTypeConverter::deleteSingletonInstance()
{
	if (_singletonInstance != NULL) delete _singletonInstance;
}


SQLRETURN OdbcTypeConverter::jsonToOdbc(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	if (TargetValuePtr == NULL) return SQL_ERROR;
	if (jsonValue->isNull())
	{
		if (StrLen_or_IndPt == NULL) return SQL_ERROR; 

		*StrLen_or_IndPt = SQL_NULL_DATA;
		return SQL_SUCCESS;
	}

	auto it = m_toOdbcConverters.find(TargetType);
	if (it == m_toOdbcConverters.end()) return SQL_ERROR; // can't convert if type is not implemented
	else return (it->second)(jsonValue, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPt);
}



/*************************************
   Individual converision functions
**************************************/

static SQLRETURN convertToOdbcVarchar(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	if (BufferLength < 0) return SQL_ERROR; // can't convert variable-length data if destination buffer size is unknown

	//TODO: Implement correct handling of multipart data.
	std::string strValue = jsonValue->asString();
	if (BufferLength < int64_t(strValue.size()) + 1) return SQL_ERROR; // TODO: Remove this line when we can handle multipart strings correctly
	memcpy((char*)TargetValuePtr, strValue.c_str(), min(int64_t(strValue.size() + 1), BufferLength));

	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = strValue.size();
	return SQL_NO_DATA;
}
