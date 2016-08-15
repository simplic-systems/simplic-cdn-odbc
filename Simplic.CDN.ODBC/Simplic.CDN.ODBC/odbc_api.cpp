#include "stdafx.h"
#include "odbc_api.h"

#include "Helper.h"
#include "util.h"
#include "Environment.h"
#include "DbConnection.h"
#include "Statement.h"

SQLAPI SQLAllocHandle(
   SQLSMALLINT   HandleType,
   SQLHANDLE     InputHandle,
   SQLHANDLE *   OutputHandlePtr)
{
	SQLAPI_DEBUG;
	// Create a new object and return a pointer to the object in OutputHandlePtr.
	// InputHandle specifies the parent of the newly created object. 
	// (Handles are just pointers to class objects in the context of this ODBC driver)

	switch (HandleType)
	{

	case SQL_HANDLE_ENV:
		*OutputHandlePtr = (SQLHANDLE) new Environment();
		return SQL_SUCCESS;

	case SQL_HANDLE_DBC:
		if (InputHandle == NULL) return SQL_ERROR;
		*OutputHandlePtr = (SQLHANDLE) new DbConnection((Environment*)InputHandle);
		return SQL_SUCCESS;

	case SQL_HANDLE_STMT:
		if (InputHandle == NULL) return SQL_ERROR;
		*OutputHandlePtr = (SQLHANDLE) new Statement((DbConnection*)InputHandle);
		return SQL_SUCCESS;

	case SQL_HANDLE_DESC:
		// TODO Descriptors not implemented
		*OutputHandlePtr = NULL;
		return SQL_ERROR;

	default:
		*OutputHandlePtr = NULL;
		return SQL_ERROR;

	}
}


SQLAPI SQLFreeHandle(
    SQLSMALLINT   HandleType,
    SQLHANDLE     Handle)
{
	SQLAPI_DEBUG
	switch (HandleType)
	{

	case SQL_HANDLE_ENV:
	case SQL_HANDLE_DBC:
	case SQL_HANDLE_STMT:
		if (Handle == NULL) return SQL_ERROR;
		delete Handle; // the object's destructor will take care of the cleanup
		return SQL_SUCCESS;
	case SQL_HANDLE_DESC:
		// TODO Descriptors not implemented
		return SQL_ERROR;

	default:
		return SQL_ERROR;

	}
}


/* ******************************
 *    DIAGNOSTICS
 * ****************************** */

SQLAPI SQLGetDiagField(
        SQLSMALLINT     HandleType,
        SQLHANDLE       Handle,
        SQLSMALLINT     RecNumber,
        SQLSMALLINT     DiagIdentifier,
        SQLPOINTER      DiagInfoPtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr)
{
	SQLAPI_DEBUG;
	//FIXME: IMPLEMENT
	if (DiagIdentifier == SQL_DIAG_SQLSTATE)
	{
		Helper::stringToOdbc("01000", (char*)DiagInfoPtr, BufferLength, (uint16_t*)StringLengthPtr);
	}
	else if (BufferLength <= 4)
	{
		*(uint16_t*)DiagInfoPtr = 0;
		if (StringLengthPtr != NULL) *StringLengthPtr = 0;
	}
	else Helper::stringToOdbc("Simplic.CDN.ODBC driver: Diagnostics not implemented yet", (char*)DiagInfoPtr, BufferLength, (uint16_t*)StringLengthPtr);
    return SQL_NO_DATA;
}





SQLAPI SQLGetDiagRec(
        SQLSMALLINT     HandleType,
        SQLHANDLE       Handle,
        SQLSMALLINT     RecNumber,
        SQLCHAR *       SQLState,
        SQLINTEGER *    NativeErrorPtr,
        SQLCHAR *       MessageText,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   TextLengthPtr)
{
	SQLAPI_DEBUG;
    //FIXME: IMPLEMENT
	if(SQLState != NULL) strcpy_s((char*) SQLState, 6, "01000"); // general warning
	if (NativeErrorPtr != NULL) *NativeErrorPtr = 0;
	Helper::stringToOdbc("Simplic.CDN.ODBC driver: Diagnostics not implemented yet", (char*)MessageText, BufferLength, (uint16_t*)TextLengthPtr);
	
    return SQL_NO_DATA;
}
