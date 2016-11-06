#pragma once
#include "odbc_api.h"
#include "Info.h"

#include <string>
#include <vector>

class DiagRecord;


/* Class that represents all diagnostic information associated with a handle.
   It can currently store a single record only. */
class DiagInfo
{
private:
	DiagRecord *m_record;

public:
	DiagInfo();
	~DiagInfo();

	/* Set the stored diag record to the record supplied in the argument, 
	   passing the ownership of the DiagRecord to the DiagInfo instance. */
	void setRecord(DiagRecord *rec);

	void clear();

	SQLRETURN toOdbc(
		SQLSMALLINT recordNum, 
		SQLSMALLINT fieldId,
		SQLPOINTER buffer, 
		SQLSMALLINT bufferLength, 
		SQLSMALLINT *dataLengthPtr);
};



class DiagRecord
{
private:

	InfoRecord m_info;

	// true iff this error/warning comes directly from Simplic.CDN
	// false if it comes from the driver.
	bool m_originatesFromDataSource;
	
	// "native" Driver/Datasource-specific error code. 
	int m_errorCode;

	std::string m_sqlState;
	std::string m_message;

public:

	enum NativeErrorCode
	{
		ERROR_OK = 0,
		ERROR_UNDEFINED = 1 // some error we didn't specify a code for.
	};

	enum Origin
	{
		ORIGIN_DATASOURCE = 0,
		ORIGIN_DRIVER = 1
	};

	DiagRecord();
	DiagRecord(std::string message);
	DiagRecord(Origin origin, std::string message);
	DiagRecord(Origin origin, std::string message, std::string sqlState, NativeErrorCode nativeError = ERROR_UNDEFINED);

	// Returns a formatted diagnostic message as specified in https://msdn.microsoft.com/en-us/library/ms713606(v=vs.85).aspx
	std::string getFormattedMessage();
	inline int getNativeErrorCode() { return m_errorCode; }
	inline std::string getSqlState() { return m_sqlState; }

	bool toOdbc(SQLSMALLINT fieldId, SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT * dataLengthPtr);
};