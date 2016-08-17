#include "stdafx.h"
#include "OdbcTypeConverter.h"
#include "Helper.h"
#include <sql.h>
#include <sqlext.h>

OdbcTypeConverter* OdbcTypeConverter::_singletonInstance;

SQLRETURN convertToOdbcVarchar(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcSmallInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);


OdbcTypeConverter::OdbcTypeConverter()
{
	// add all conversion functions we want to use 
	m_toOdbcConverters[SQL_VARCHAR] = JsonToOdbcConverter(convertToOdbcVarchar);
	m_toOdbcConverters[SQL_CHAR] = JsonToOdbcConverter(convertToOdbcVarchar);
	m_toOdbcConverters[SQL_INTEGER ] = JsonToOdbcConverter(convertToOdbcInteger);
	m_toOdbcConverters[SQL_SMALLINT] = JsonToOdbcConverter(convertToOdbcSmallInteger);
	m_toOdbcConverters[-15] = JsonToOdbcConverter(convertToOdbcSmallInteger); //TODO: HACK; find out what this "-15" type actually means!

	// Column lengths by type.
	// Note: This list of types is not complete, the length of some types depends on precision/length parameters.
	m_columnSizes[SQL_BIT] = 1;
	m_columnSizes[SQL_TINYINT] = 3;
	m_columnSizes[SQL_SMALLINT] = 5;
	m_columnSizes[SQL_SMALLINT] = 5;
	m_columnSizes[SQL_INTEGER] = 10;
	m_columnSizes[SQL_BIGINT] = 20; // should be 19 if unsigned but we can't differentiate that via the type field
	m_columnSizes[SQL_REAL] = 7;
	m_columnSizes[SQL_FLOAT] = 15;
	m_columnSizes[SQL_DOUBLE] = 15;
	m_columnSizes[SQL_TYPE_DATE] = 10;

	// defaults for string data types.
	m_columnSizes[SQL_CHAR] = 256;
	m_columnSizes[SQL_VARCHAR] = 256;
}


void OdbcTypeConverter::createSingletonInstance()
{
	_singletonInstance = new OdbcTypeConverter();
}

void OdbcTypeConverter::deleteSingletonInstance()
{
	if (_singletonInstance != NULL) delete _singletonInstance;
}

SQLLEN OdbcTypeConverter::getColumnSizeByType(SQLSMALLINT type)
{
	auto it = m_columnSizes.find(type);
	if (it == m_columnSizes.end()) return 0;
	else return it->second;
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
	if (it == m_toOdbcConverters.end())
	{
		return SQL_ERROR; // can't convert if type is not implemented
	}
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
	return SQL_SUCCESS;
}

static SQLRETURN convertToOdbcInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(int*)TargetValuePtr = jsonValue->asInt();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(int);
	return SQL_SUCCESS;
}

static SQLRETURN convertToOdbcSmallInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(SQLSMALLINT*)TargetValuePtr = (SQLSMALLINT)jsonValue->asInt();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(SQLSMALLINT);
	return SQL_SUCCESS;
}