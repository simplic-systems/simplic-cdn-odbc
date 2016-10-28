#include "stdafx.h"
#include "DiagInfo.h"


DiagInfo::DiagInfo()
{
	m_record = NULL;
}


DiagInfo::~DiagInfo()
{
	clear();
}

void DiagInfo::setRecord(DiagRecord *rec)
{
	if (m_record != NULL) delete m_record;
	m_record = rec;
}

void DiagInfo::clear()
{
	if (m_record != NULL) delete m_record;
	m_record = NULL;
}

SQLRETURN DiagInfo::toOdbc(SQLSMALLINT recordNum, SQLSMALLINT fieldId, SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT * dataLengthPtr)
{
	if (recordNum != 1 || m_record == NULL) return SQL_NO_DATA;

	return m_record->toOdbc(fieldId, buffer, bufferLength, dataLengthPtr) ? SQL_SUCCESS : SQL_ERROR;
}

DiagRecord::DiagRecord()
	: DiagRecord(ORIGIN_DRIVER, "Undefined error")
{ }

DiagRecord::DiagRecord(std::string message)
	: DiagRecord(ORIGIN_DRIVER, message)
{ }

DiagRecord::DiagRecord(Origin origin, std::string message)
	: DiagRecord(origin, message, "HY000")
{ }

DiagRecord::DiagRecord(Origin origin, std::string message, std::string sqlState, NativeErrorCode nativeError)
{
	m_originatesFromDataSource = (origin == ORIGIN_DATASOURCE);
	m_message = message;
	m_sqlState = sqlState;
	m_errorCode = nativeError;

	m_info.addField(SQL_DIAG_MESSAGE_TEXT, new StringInfoField(
		[this]() {return getFormattedMessage(); }
	));
	m_info.addField(SQL_DIAG_NATIVE, new FixedSizeInfoField<SQLINTEGER>(
		[this]() {return getNativeErrorCode(); }
	));
	m_info.addField(SQL_DIAG_SQLSTATE, new StringInfoField(
		[this]() {return getSqlState(); }
	));
}

std::string DiagRecord::getFormattedMessage()
{
	// build the message as specified in https://msdn.microsoft.com/en-us/library/ms713606(v=vs.85).aspx
	std::string result;
	result += std::string("[") + VENDOR_IDENTIFIER + "]";
	result += "[Simplic.CDN.ODBC]";
	if (m_originatesFromDataSource) 
		result += "[Simplic.CDN]";
	result += m_message;
	return result;
}

bool DiagRecord::toOdbc(SQLSMALLINT fieldId, SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT * dataLengthPtr)
{
	OdbcInfoField* field = m_info.getField(fieldId);
	if (field == NULL) return false;
	return field->toOdbc(buffer, bufferLength, dataLengthPtr);
}

