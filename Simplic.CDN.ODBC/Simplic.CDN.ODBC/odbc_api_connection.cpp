#include "stdafx.h"
#include "util.h"
#include "odbc_api.h"
#include "global_info.h"

SQLAPI SQLConnect(
        SQLHDBC        ConnectionHandle,
        SQLCHAR *      ServerName,
        SQLSMALLINT    NameLength1,
        SQLCHAR *      UserName,
        SQLSMALLINT    NameLength2,
        SQLCHAR *      Authentication,
        SQLSMALLINT    NameLength3)
{
	SQLAPI_DEBUG;
	//FIXME: IMPLEMENT
    return SQL_ERROR;
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
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLDisconnect(
        SQLHDBC     ConnectionHandle)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLGetConnectAttr(
        SQLHDBC        ConnectionHandle,
        SQLINTEGER     Attribute,
        SQLPOINTER     ValuePtr,
        SQLINTEGER     BufferLength,
        SQLINTEGER *   StringLengthPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLSetConnectAttr(
        SQLHDBC       ConnectionHandle,
        SQLINTEGER    Attribute,
        SQLPOINTER    ValuePtr,
        SQLINTEGER    StringLength)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLGetFunctions(
        SQLHDBC           ConnectionHandle,
        SQLUSMALLINT      FunctionId,
        SQLUSMALLINT *    SupportedPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}




SQLAPI SQLGetInfo(
        SQLHDBC         ConnectionHandle,
        SQLUSMALLINT    InfoType,
        SQLPOINTER      InfoValuePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr)
{
	SQLAPI_DEBUG;
	return GlobalInfo::getInstance()->getInfo()->getInfo(
		ConnectionHandle,
		InfoType,
		InfoValuePtr,
		BufferLength,
		StringLengthPtr
	);
}






SQLAPI SQLNativeSql(
        SQLHDBC        ConnectionHandle,
        SQLCHAR *      InStatementText,
        SQLINTEGER     TextLength1,
        SQLCHAR *      OutStatementText,
        SQLINTEGER     BufferLength,
        SQLINTEGER *   TextLength2Ptr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLEndTran(
        SQLSMALLINT   HandleType,
        SQLHANDLE     Handle,
        SQLSMALLINT   CompletionType)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}








