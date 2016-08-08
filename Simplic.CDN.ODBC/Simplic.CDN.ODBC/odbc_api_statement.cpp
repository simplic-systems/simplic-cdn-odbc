#include "stdafx.h"
#include "util.h"
#include "odbc_api.h"

/* ******************************
 *    RESULT SET BINDING
 * ****************************** */

SQLAPI SQLBindCol(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   ColumnNumber,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_Ind)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}


/* ******************************
 *    PARAMETER BUFFER FUNCTIONS
 * ****************************** */

SQLAPI SQLBindParameter(
        SQLHSTMT        StatementHandle,
        SQLUSMALLINT    ParameterNumber,
        SQLSMALLINT     InputOutputType,
        SQLSMALLINT     ValueType,
        SQLSMALLINT     ParameterType,
        SQLULEN         ColumnSize,
        SQLSMALLINT     DecimalDigits,
        SQLPOINTER      ParameterValuePtr,
        SQLLEN          BufferLength,
        SQLLEN *        StrLen_or_IndPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}



SQLAPI SQLNumParams(
        SQLHSTMT        StatementHandle,
        SQLSMALLINT *   ParameterCountPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}



SQLAPI SQLParamData(
        SQLHSTMT       StatementHandle,
        SQLPOINTER *   ValuePtrPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}




SQLAPI  SQLPutData(
        SQLHSTMT     StatementHandle,
        SQLPOINTER   DataPtr,
        SQLLEN       StrLen_or_Ind)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






/* ******************************
 *    CURSOR FUNCTIONS
 * ****************************** */

SQLAPI  SQLCloseCursor(
     SQLHSTMT     StatementHandle)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}




SQLAPI SQLGetCursorName(
     SQLHSTMT        StatementHandle,
     SQLCHAR *       CursorName,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   NameLengthPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}




SQLAPI SQLSetCursorName(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CursorName,
     SQLSMALLINT   NameLength)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





/* ******************************
 *    METADATA QUERY FUNCTIONS
 * ****************************** */

SQLAPI SQLColAttribute (
        SQLHSTMT        StatementHandle,
        SQLUSMALLINT    ColumnNumber,
        SQLUSMALLINT    FieldIdentifier,
        SQLPOINTER      CharacterAttributePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr,
        SQLLEN *        NumericAttributePtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLDescribeCol(
        SQLHSTMT       StatementHandle,
        SQLUSMALLINT   ColumnNumber,
        SQLCHAR *      ColumnName,
        SQLSMALLINT    BufferLength,
        SQLSMALLINT *  NameLengthPtr,
        SQLSMALLINT *  DataTypePtr,
        SQLULEN *      ColumnSizePtr,
        SQLSMALLINT *  DecimalDigitsPtr,
        SQLSMALLINT *  NullablePtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}


SQLAPI SQLSpecialColumns(SQLHSTMT StatementHandle,
           SQLUSMALLINT IdentifierType,
           _In_reads_opt_(NameLength1) SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
           _In_reads_opt_(NameLength2) SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
           _In_reads_opt_(NameLength3) SQLCHAR *TableName, SQLSMALLINT NameLength3,
           SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}



/*
SQLAPI SQLSpecialColumns(
        SQLHSTMT      StatementHandle,
        SQLSMALLINT   IdentifierType,
        SQLCHAR *     CatalogName,
        SQLSMALLINT   NameLength1,
        SQLCHAR *     SchemaName,
        SQLSMALLINT   NameLength2,
        SQLCHAR *     TableName,
        SQLSMALLINT   NameLength3,
        SQLSMALLINT   Scope,
        SQLSMALLINT   Nullable)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}

*/



/* ******************************
 *    RESULT SET INFORMATION
 * ****************************** */

SQLAPI SQLNumResultCols(
        SQLHSTMT        StatementHandle,
        SQLSMALLINT *   ColumnCountPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}




SQLAPI SQLRowCount(
        SQLHSTMT   StatementHandle,
        SQLLEN *   RowCountPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}




/* ******************************
 *    DATA DICTIONARY QUERY FUNCTIONS
 * ****************************** */


SQLAPI SQLColumns(
        SQLHSTMT       StatementHandle,
        SQLCHAR *      CatalogName,
        SQLSMALLINT    NameLength1,
        SQLCHAR *      SchemaName,
        SQLSMALLINT    NameLength2,
        SQLCHAR *      TableName,
        SQLSMALLINT    NameLength3,
        SQLCHAR *      ColumnName,
        SQLSMALLINT    NameLength4)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLGetTypeInfo(
        SQLHSTMT      StatementHandle,
        SQLSMALLINT   DataType)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLStatistics(
        SQLHSTMT        StatementHandle,
        SQLCHAR *       CatalogName,
        SQLSMALLINT     NameLength1,
        SQLCHAR *       SchemaName,
        SQLSMALLINT     NameLength2,
        SQLCHAR *       TableName,
        SQLSMALLINT     NameLength3,
        SQLUSMALLINT    Unique,
        SQLUSMALLINT    Reserved)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLTables(
        SQLHSTMT       StatementHandle,
        SQLCHAR *      CatalogName,
        SQLSMALLINT    NameLength1,
        SQLCHAR *      SchemaName,
        SQLSMALLINT    NameLength2,
        SQLCHAR *      TableName,
        SQLSMALLINT    NameLength3,
        SQLCHAR *      TableType,
        SQLSMALLINT    NameLength4)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






/* ******************************
 *    PREPARE & EXECUTE
 * ****************************** */

SQLAPI SQLExecute(
        SQLHSTMT     StatementHandle)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLExecDirect(
        SQLHSTMT     StatementHandle,
        SQLCHAR *    StatementText,
        SQLINTEGER   TextLength)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLPrepare(
        SQLHSTMT      StatementHandle,
        SQLCHAR *     StatementText,
        SQLINTEGER    TextLength)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLCancel(
        SQLHSTMT     StatementHandle)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






/* ******************************
 *    DATA RETRIEVAL FROM RESULT SET
 * ****************************** */

SQLAPI SQLFetch(
        SQLHSTMT     StatementHandle)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLFetchScroll(
        SQLHSTMT      StatementHandle,
        SQLSMALLINT   FetchOrientation,
        SQLLEN        FetchOffset)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLGetData(
        SQLHSTMT       StatementHandle,
        SQLUSMALLINT   Col_or_Param_Num,
        SQLSMALLINT    TargetType,
        SQLPOINTER     TargetValuePtr,
        SQLLEN         BufferLength,
        SQLLEN *       StrLen_or_IndPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}







/* ******************************
 *    ATTRIBUTES
 * ****************************** */


SQLAPI SQLGetStmtAttr(
        SQLHSTMT        StatementHandle,
        SQLINTEGER      Attribute,
        SQLPOINTER      ValuePtr,
        SQLINTEGER      BufferLength,
        SQLINTEGER *    StringLengthPtr)
{
	/*
	10010 SQL_ATTR_APP_ROW_DESC
	10011 SQL_ATTR_APP_PARAM_DESC
	10012 SQL_ATTR_IMP_ROW_DESC
	10013 SQL_ATTR_IMP_PARAM_DESC
	*/
	SQLAPI_DEBUG;
	if (StringLengthPtr != NULL) *StringLengthPtr = 0; 
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}





SQLAPI SQLSetStmtAttr(
        SQLHSTMT      StatementHandle,
        SQLINTEGER    Attribute,
        SQLPOINTER    ValuePtr,
        SQLINTEGER    StringLength)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}












