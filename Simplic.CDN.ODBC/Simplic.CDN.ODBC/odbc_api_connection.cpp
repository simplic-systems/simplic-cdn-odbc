#include "stdafx.h"
#include "util.h"
#include "odbc_api.h"
#include "sqlext.h"
#include "global_info.h"
#include "DSN.h"
#include "DbConnection.h"


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
	SQLAPI_DEBUG;
	
	// create DSN from connection string
	DSN dsn;
	std::string connstr;
	if (StringLength1 >= 0)
	{
		connstr = std::string((char*)InConnectionString, StringLength1);
	}
	else
	{
		// StringLength1 negative => assume that InConnectionString is null-terminated
		connstr = std::string((char*)InConnectionString);
	}

	if (!dsn.fromConnectionString(connstr)) return SQL_ERROR;

	dsn.loadAttributesFromRegistry();
	if (WindowHandle) dsn.showConfigDialog(WindowHandle);

	// write connection string to buffer
	if (OutConnectionString != NULL)
	{
		Util::stringToOdbcBuffer(dsn.toConnectionString(), OutConnectionString, BufferLength, StringLength2Ptr);
	}

	// connect to the url/user/pw from that DSN
	DbConnection* dbc = (DbConnection*)ConnectionHandle;
	bool connected = dbc->connect(dsn.getUrl(), dsn.getUser(), dsn.getPassword());

    return connected ? SQL_SUCCESS : SQL_ERROR;
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
	SQLAPI_DEBUG;

	DbConnection* dbc = (DbConnection*) ConnectionHandle;
	switch (Attribute)
	{
	case SQL_LOGIN_TIMEOUT: dbc->setTimeout(uint32_t(ValuePtr)); return SQL_SUCCESS;

	default: return SQL_ERROR;
	}
}






SQLAPI SQLGetInfo(
        SQLHDBC         ConnectionHandle,
        SQLUSMALLINT    InfoType,
        SQLPOINTER      InfoValuePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr)
{
	SQLAPI_DEBUG;
	OdbcInfoField* infoField = GlobalInfo::getInstance()->getInfoField((DbConnection*)ConnectionHandle, InfoType);
	if (infoField == NULL) return SQL_ERROR;
	bool success = infoField->toOdbc(
		InfoValuePtr,
		BufferLength,
		StringLengthPtr
	);
	return success ? SQL_SUCCESS : SQL_ERROR;
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








