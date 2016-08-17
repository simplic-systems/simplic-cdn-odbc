#pragma once

#include "json/json.h"

#include <sqltypes.h>
#include <map>
#include <functional>

typedef std::function<SQLRETURN(
	const Json::Value* jsonValue,
	SQLSMALLINT TargetType,
	SQLPOINTER TargetValuePtr,
	SQLLEN BufferLength,
	SQLLEN *StrLen_or_IndPtr)> JsonToOdbcConverter;

class OdbcTypeConverter
{
private:
	static OdbcTypeConverter* _singletonInstance;

	// Maps SQL types to converter functions.
	std::map < SQLUSMALLINT, JsonToOdbcConverter> m_toOdbcConverters;

	// Maps SQL types to column sizes. See https://msdn.microsoft.com/de-de/library/ms711786(v=vs.85).aspx 
	std::map < SQLUSMALLINT, SQLLEN> m_columnSizes;

	OdbcTypeConverter();

public:

	static void OdbcTypeConverter::createSingletonInstance();
	static void OdbcTypeConverter::deleteSingletonInstance();

	/// Returns a pointer to the singleton instance, or null if it does not exist yet.
	static inline OdbcTypeConverter* getInstance() { return _singletonInstance; }

	SQLRETURN jsonToOdbc(
		const Json::Value* jsonValue,
		SQLSMALLINT TargetType,
		SQLPOINTER TargetValuePtr,
		SQLLEN BufferLength,
		SQLLEN *StrLen_or_IndPt);


	SQLLEN getColumnSizeByType(SQLSMALLINT type);

};

