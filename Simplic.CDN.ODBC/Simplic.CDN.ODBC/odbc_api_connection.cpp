#include "stdafx.h"
#include "odbc_api.h"

SQLAPI SQLConnect(
        SQLHDBC        ConnectionHandle,
        SQLCHAR *      ServerName,
        SQLSMALLINT    NameLength1,
        SQLCHAR *      UserName,
        SQLSMALLINT    NameLength2,
        SQLCHAR *      Authentication,
        SQLSMALLINT    NameLength3)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLDriverConnect(
        SQLHDBC         ConnectionHandle,
        SQLHWND         WindowHandle,
        SQLCHAR *       InConnectionString,
        SQLSMALLINT     StringLength1,
        SQLCHAR *       OutConnectionString,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLength2Ptr,
        SQLUSMALLINT    DriverCompletion)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLDisconnect(
        SQLHDBC     ConnectionHandle)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLGetConnectAttr(
        SQLHDBC        ConnectionHandle,
        SQLINTEGER     Attribute,
        SQLPOINTER     ValuePtr,
        SQLINTEGER     BufferLength,
        SQLINTEGER *   StringLengthPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLSetConnectAttr(
        SQLHDBC       ConnectionHandle,
        SQLINTEGER    Attribute,
        SQLPOINTER    ValuePtr,
        SQLINTEGER    StringLength)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLGetFunctions(
        SQLHDBC           ConnectionHandle,
        SQLUSMALLINT      FunctionId,
        SQLUSMALLINT *    SupportedPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}




SQLAPI SQLGetInfo(
        SQLHDBC         ConnectionHandle,
        SQLUSMALLINT    InfoType,
        SQLPOINTER      InfoValuePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLNativeSql(
        SQLHDBC        ConnectionHandle,
        SQLCHAR *      InStatementText,
        SQLINTEGER     TextLength1,
        SQLCHAR *      OutStatementText,
        SQLINTEGER     BufferLength,
        SQLINTEGER *   TextLength2Ptr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLEndTran(
        SQLSMALLINT   HandleType,
        SQLHANDLE     Handle,
        SQLSMALLINT   CompletionType)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}








