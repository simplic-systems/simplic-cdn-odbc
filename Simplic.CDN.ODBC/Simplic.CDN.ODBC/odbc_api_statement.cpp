#include "stdafx.h"
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}



SQLAPI SQLNumParams(
        SQLHSTMT        StatementHandle,
        SQLSMALLINT *   ParameterCountPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}



SQLAPI SQLParamData(
        SQLHSTMT       StatementHandle,
        SQLPOINTER *   ValuePtrPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}




SQLAPI  SQLPutData(
        SQLHSTMT     StatementHandle,
        SQLPOINTER   DataPtr,
        SQLLEN       StrLen_or_Ind)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






/* ******************************
 *    CURSOR FUNCTIONS
 * ****************************** */

SQLAPI  SQLCloseCursor(
     SQLHSTMT     StatementHandle)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}




SQLAPI SQLGetCursorName(
     SQLHSTMT        StatementHandle,
     SQLCHAR *       CursorName,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   NameLengthPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}




SQLAPI SQLSetCursorName(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CursorName,
     SQLSMALLINT   NameLength)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}


SQLAPI SQLSpecialColumns(SQLHSTMT StatementHandle,
           SQLUSMALLINT IdentifierType,
           _In_reads_opt_(NameLength1) SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
           _In_reads_opt_(NameLength2) SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
           _In_reads_opt_(NameLength3) SQLCHAR *TableName, SQLSMALLINT NameLength3,
           SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}

*/



/* ******************************
 *    RESULT SET INFORMATION
 * ****************************** */

SQLAPI SQLNumResultCols(
        SQLHSTMT        StatementHandle,
        SQLSMALLINT *   ColumnCountPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}




SQLAPI SQLRowCount(
        SQLHSTMT   StatementHandle,
        SQLLEN *   RowCountPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLGetTypeInfo(
        SQLHSTMT      StatementHandle,
        SQLSMALLINT   DataType)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






/* ******************************
 *    PREPARE & EXECUTE
 * ****************************** */

SQLAPI SQLExecute(
        SQLHSTMT     StatementHandle)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLExecDirect(
        SQLHSTMT     StatementHandle,
        SQLCHAR *    StatementText,
        SQLINTEGER   TextLength)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLPrepare(
        SQLHSTMT      StatementHandle,
        SQLCHAR *     StatementText,
        SQLINTEGER    TextLength)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLCancel(
        SQLHSTMT     StatementHandle)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






/* ******************************
 *    DATA RETRIEVAL FROM RESULT SET
 * ****************************** */

SQLAPI SQLFetch(
        SQLHSTMT     StatementHandle)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLFetchScroll(
        SQLHSTMT      StatementHandle,
        SQLSMALLINT   FetchOrientation,
        SQLLEN        FetchOffset)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLGetData(
        SQLHSTMT       StatementHandle,
        SQLUSMALLINT   Col_or_Param_Num,
        SQLSMALLINT    TargetType,
        SQLPOINTER     TargetValuePtr,
        SQLLEN         BufferLength,
        SQLLEN *       StrLen_or_IndPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}





SQLAPI SQLSetStmtAttr(
        SQLHSTMT      StatementHandle,
        SQLINTEGER    Attribute,
        SQLPOINTER    ValuePtr,
        SQLINTEGER    StringLength)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}












