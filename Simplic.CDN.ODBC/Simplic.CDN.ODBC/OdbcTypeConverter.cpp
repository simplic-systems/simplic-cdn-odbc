#include "stdafx.h"
#include "OdbcTypeConverter.h"
#include "Helper.h"
#include "odbc_api.h"
#include <sqlext.h>
#include <stdint.h>

OdbcTypeConverter* OdbcTypeConverter::_singletonInstance;

SQLRETURN convertToOdbcVarchar(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);

SQLRETURN convertToOdbcLongInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcSmallInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcTinyInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);

SQLRETURN convertToOdbcUnsignedLongInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcUnsignedInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcUnsignedSmallInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);
SQLRETURN convertToOdbcUnsignedTinyInteger(const Json::Value* jsonValue, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPt);


OdbcTypeConverter::OdbcTypeConverter()
{
	// add all conversion functions we want to use 
	// Note: In ODBC terminology, the different-length integers are defined as follows:
	// BIGINT = 64 bits
	// INTEGER = 32 bits
	// LONG = 32 bits
	// SHORT = 16 bits
	// TINY = 8 bits

	// --- SQL data types ---
	m_toOdbcConverters[SQL_VARCHAR] = JsonToOdbcConverter(convertToOdbcVarchar);
	m_toOdbcConverters[SQL_CHAR] = JsonToOdbcConverter(convertToOdbcVarchar);

	m_toOdbcConverters[SQL_BIGINT] = JsonToOdbcConverter(convertToOdbcLongInteger);
	m_toOdbcConverters[SQL_INTEGER ] = JsonToOdbcConverter(convertToOdbcInteger);
	m_toOdbcConverters[SQL_SMALLINT] = JsonToOdbcConverter(convertToOdbcSmallInteger);
	m_toOdbcConverters[SQL_TINYINT] = JsonToOdbcConverter(convertToOdbcTinyInteger);


	// --- C data types ---
	m_toOdbcConverters[SQL_C_SBIGINT] = JsonToOdbcConverter(convertToOdbcLongInteger);
	m_toOdbcConverters[SQL_C_SLONG]   = JsonToOdbcConverter(convertToOdbcInteger);
	m_toOdbcConverters[SQL_C_SSHORT] = JsonToOdbcConverter(convertToOdbcSmallInteger);
	m_toOdbcConverters[SQL_C_STINYINT] = JsonToOdbcConverter(convertToOdbcTinyInteger);

	m_toOdbcConverters[SQL_C_UBIGINT]  = JsonToOdbcConverter(convertToOdbcUnsignedLongInteger);
	m_toOdbcConverters[SQL_C_ULONG]    = JsonToOdbcConverter(convertToOdbcUnsignedInteger);
	m_toOdbcConverters[SQL_C_USHORT]   = JsonToOdbcConverter(convertToOdbcUnsignedSmallInteger);
	m_toOdbcConverters[SQL_C_UTINYINT] = JsonToOdbcConverter(convertToOdbcUnsignedTinyInteger);


	// Column lengths by type. See https://msdn.microsoft.com/en-us/library/ms711786(v=vs.85).aspx
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

static SQLRETURN convertToOdbcLongInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(int64_t*)TargetValuePtr = jsonValue->asInt64();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(int64_t);
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

static SQLRETURN convertToOdbcTinyInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(int8_t*)TargetValuePtr = (int8_t)jsonValue->asInt();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(int8_t);
	return SQL_SUCCESS;
}






static SQLRETURN convertToOdbcUnsignedLongInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(uint64_t*)TargetValuePtr = jsonValue->asUInt64();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(uint64_t);
	return SQL_SUCCESS;
}


static SQLRETURN convertToOdbcUnsignedInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(unsigned int*)TargetValuePtr = jsonValue->asUInt();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(unsigned int);
	return SQL_SUCCESS;
}


static SQLRETURN convertToOdbcUnsignedSmallInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(SQLUSMALLINT*)TargetValuePtr = (SQLUSMALLINT)jsonValue->asUInt();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(SQLUSMALLINT);
	return SQL_SUCCESS;
}

static SQLRETURN convertToOdbcUnsignedTinyInteger(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPt)
{
	*(uint8_t*)TargetValuePtr = (uint8_t)jsonValue->asUInt();
	if (StrLen_or_IndPt != NULL) *StrLen_or_IndPt = sizeof(uint8_t);
	return SQL_SUCCESS;
}