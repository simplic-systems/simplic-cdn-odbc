#include "stdafx.h"
#include "odbc_api.h"

SQLAPI SQLAllocHandle(
   SQLSMALLINT   HandleType,
   SQLHANDLE     InputHandle,
   SQLHANDLE *   OutputHandlePtr)
{
   // FIXME: Implement
   return SQL_SUCCESS;
}


SQLAPI SQLFreeHandle(
    SQLSMALLINT   HandleType,
    SQLHANDLE     Handle)
{
    // FIXME: Implement
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}
