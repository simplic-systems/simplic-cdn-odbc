#include "stdafx.h"
#include "odbc_api.h"

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
    //FIXME: IMPLEMENT
	SQLAPI_DEBUG
    return SQL_SUCCESS;
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
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_NO_DATA;
}
