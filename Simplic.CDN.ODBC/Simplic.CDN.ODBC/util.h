#pragma once

#include "odbc_api.h"
#include <string>

// this macro allows us to set a single breakpoint that will trigger when any
// ODBC API function is called.
#ifdef _DEBUG
#define SQLAPI_DEBUG odbc_api_called(__FILE__, __LINE__, __FUNCTION__);
void odbc_api_called(const char* file, int line, const char* function);
#else
#define SQLAPI_DEBUG /*nop*/
#endif

class Util
{
private:
	Util();
public:
	static bool stringToOdbcBuffer(std::string value, SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr = 0);
	template<typename T> static bool toOdbcBuffer(const T& value, SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr = 0);

};


template<typename T>
inline bool Util::toOdbcBuffer(const T& value, SQLPOINTER buffer, SQLSMALLINT BufferLength, SQLSMALLINT * StringLengthPtr)
{
	if(StringLengthPtr != NULL) *StringLengthPtr = sizeof(value);
	SQLSMALLINT bytesToCopy = min(SQLSMALLINT(sizeof(value)), BufferLength);
	memcpy(buffer, &value, bytesToCopy);

	return bytesToCopy <= BufferLength;
}
