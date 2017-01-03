#include "stdafx.h"
#include "util.h"
#include "odbc_api.h"
#include <sqlext.h>

#include "Helper.h"
#include "Statement.h"
#include "QueryResult.h"

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
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		bool result = stmt->bindColumn(
			ColumnNumber,
			TargetType,
			BufferLength,
			(void*)TargetValuePtr,
			StrLen_or_Ind
		);
		return result ? SQL_SUCCESS : SQL_ERROR;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
	SQLAPI_DEBUG;
    //FIXME: IMPLEMENT

	if (InputOutputType != SQL_PARAM_INPUT)
		return SQL_ERROR;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;
	try
	{
		// TODO: handle conversion based on ValueType and ParameterType
		bool result = stmt->bindParameter(
			ParameterNumber,
			ParameterType,
			BufferLength,
			(void*)ParameterValuePtr,
			StrLen_or_IndPtr
		);
		return result ? SQL_SUCCESS : SQL_ERROR;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		SQLRETURN result = stmt->paramData(ValuePtrPtr);
		return result;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
}




SQLAPI  SQLPutData(
        SQLHSTMT     StatementHandle,
        SQLPOINTER   DataPtr,
        SQLLEN       StrLen_or_Ind)
{
	SQLAPI_DEBUG
	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		bool result = stmt->putData(DataPtr, StrLen_or_Ind);
		return result ? SQL_SUCCESS : SQL_ERROR;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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

#ifdef _WIN64
SQLAPI SQLColAttribute(
	SQLHSTMT        StatementHandle,
	SQLUSMALLINT    ColumnNumber,
	SQLUSMALLINT    FieldIdentifier,
	SQLPOINTER      CharacterAttributePtr,
	SQLSMALLINT     BufferLength,
	SQLSMALLINT *   StringLengthPtr,
	SQLLEN *        NumericAttributePtr)
#else
SQLAPI SQLColAttribute (
        SQLHSTMT        StatementHandle,
        SQLUSMALLINT    ColumnNumber,
        SQLUSMALLINT    FieldIdentifier,
        SQLPOINTER      CharacterAttributePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr,
        SQLPOINTER      NumericAttributePtr)
#endif
{
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		ColumnDescriptor* col = stmt->getColumnDescriptor(ColumnNumber);
		if (col == NULL) return SQL_ERROR;

		bool result = col->odbcGetField(
			FieldIdentifier,
			CharacterAttributePtr,
			BufferLength,
			StringLengthPtr,
			(SQLLEN*)NumericAttributePtr);

		return result ? SQL_SUCCESS : SQL_ERROR;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
}





SQLAPI SQLDescribeCol(
        SQLHSTMT       StatementHandle,
        SQLUSMALLINT   ColumnNumber, 
        SQLCHAR *      ColumnName,   // ok
        SQLSMALLINT    BufferLength, // ""
        SQLSMALLINT *  NameLengthPtr, // ""
        SQLSMALLINT *  DataTypePtr, // ok
        SQLULEN *      ColumnSizePtr, // not implemented
        SQLSMALLINT *  DecimalDigitsPtr, // not implemented
        SQLSMALLINT *  NullablePtr) // not implemented
{
	SQLAPI_DEBUG;
	
	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		ColumnDescriptor* col = stmt->getColumnDescriptor(ColumnNumber);
		if (col == NULL) return SQL_ERROR;

		std::string name = col->getName();
		if (ColumnName != NULL)
		{
			memcpy(ColumnName, name.c_str(), min(name.size() + 1, BufferLength - 1));

			// add terminating zero char if we didn't manage to copy it from the name.
			if (BufferLength <= name.size()) ColumnName[BufferLength - 1] = '\0';
		}

		if (NameLengthPtr != NULL) *NameLengthPtr = SQLUSMALLINT(min(name.size(), 0xFFFF));
		if (DataTypePtr != NULL) *DataTypePtr = col->getType();


		// set properties we didn't implement yet to "unknown"
		if (ColumnSizePtr != NULL) *ColumnSizePtr = col->getSize();
		if (DecimalDigitsPtr != NULL) *DecimalDigitsPtr = 0;
		if (NullablePtr != NULL) *NullablePtr = SQL_NULLABLE_UNKNOWN;

		
		return SQL_SUCCESS;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL || RowCountPtr == NULL) return SQL_ERROR;

	try
	{
		*RowCountPtr = stmt->getNumAffectedRows();
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }

	return SQL_SUCCESS;
}




/* ******************************
 *    DATA DICTIONARY QUERY FUNCTIONS
 * ****************************** */


SQLAPI SQLColumns(
        SQLHSTMT       StatementHandle,
        SQLCHAR *      pCatalogName,
        SQLSMALLINT    NameLength1,
        SQLCHAR *      pSchemaName,
        SQLSMALLINT    NameLength2,
        SQLCHAR *      pTableName,
        SQLSMALLINT    NameLength3,
        SQLCHAR *      pColumnName,
        SQLSMALLINT    NameLength4)
{
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		std::string catalogName = Helper::stringFromOdbc((char*)pCatalogName, NameLength1);
		std::string schemaName = Helper::stringFromOdbc((char*)pSchemaName, NameLength2);
		std::string tableName = Helper::stringFromOdbc((char*)pTableName, NameLength3);
		std::string columnName = Helper::stringFromOdbc((char*)pColumnName, NameLength4);

		SQLRETURN result = stmt->getColumns(catalogName, schemaName, tableName, columnName);
		return result;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
        SQLCHAR *      pCatalogName,
        SQLSMALLINT    NameLength1,
        SQLCHAR *      pSchemaName,
        SQLSMALLINT    NameLength2,
        SQLCHAR *      pTableName,
        SQLSMALLINT    NameLength3,
        SQLCHAR *      pTableType,
        SQLSMALLINT    NameLength4)
{
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		std::string catalogName = Helper::stringFromOdbc((char*)pCatalogName, NameLength1);
		std::string schemaName = Helper::stringFromOdbc((char*)pSchemaName, NameLength2);
		std::string tableName = Helper::stringFromOdbc((char*)pTableName, NameLength3);
		std::string tableType = Helper::stringFromOdbc((char*)pTableType, NameLength4);

		bool result = stmt->getTables(catalogName, schemaName, tableName, tableType);
		return result ? SQL_SUCCESS : SQL_ERROR;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
}






/* ******************************
 *    PREPARE & EXECUTE
 * ****************************** */

SQLAPI SQLExecute(
        SQLHSTMT     StatementHandle)
{
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		SQLRETURN result = stmt->execute();
		return result;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
}





SQLAPI SQLExecDirect(
        SQLHSTMT     StatementHandle,
        SQLCHAR *    StatementText,
        SQLINTEGER   TextLength)
{
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		std::string query = Helper::stringFromOdbc((char*)StatementText, TextLength);
		stmt->setQuery(query);

		SQLRETURN result = stmt->execute();
		return result;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
}





SQLAPI SQLPrepare(
        SQLHSTMT      StatementHandle,
        SQLCHAR *     StatementText,
        SQLINTEGER    TextLength)
{
	SQLAPI_DEBUG;
	
	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;
	
	try
	{
		std::string query = Helper::stringFromOdbc((char*)StatementText, TextLength);
		stmt->setQuery(query);

		return SQL_SUCCESS;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
	SQLAPI_DEBUG;

	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		bool result = stmt->fetchNext();
		if (!result) return SQL_ERROR;
		if (stmt->getNumFetchedRows() == 0) return SQL_NO_DATA;
		return SQL_SUCCESS;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
	SQLAPI_DEBUG;
	Statement* stmt = (Statement*)StatementHandle;
	if (stmt == NULL) return SQL_ERROR;

	try
	{
		SQLRETURN result = stmt->getData(Col_or_Param_Num, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPtr);
		return result;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, stmt); return SQL_ERROR; }
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
	*((uint64_t*)ValuePtr) = 0x123456789AAAULL;
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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












