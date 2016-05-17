#include "stdafx.h"
#include "util.h"
#include "odbc_api.h"

SQLAPI SQLCopyDesc(
        SQLHDESC     SourceDescHandle,
        SQLHDESC     TargetDescHandle)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLGetDescField(
        SQLHDESC        DescriptorHandle,
        SQLSMALLINT     RecNumber,
        SQLSMALLINT     FieldIdentifier,
        SQLPOINTER      ValuePtr,
        SQLINTEGER      BufferLength,
        SQLINTEGER *    StringLengthPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLSetDescField(
        SQLHDESC      DescriptorHandle,
        SQLSMALLINT   RecNumber,
        SQLSMALLINT   FieldIdentifier,
        SQLPOINTER    ValuePtr,
        SQLINTEGER    BufferLength)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLGetDescRec(
        SQLHDESC        DescriptorHandle,
        SQLSMALLINT     RecNumber,
        SQLCHAR *       Name,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *   StringLengthPtr,
        SQLSMALLINT *   TypePtr,
        SQLSMALLINT *   SubTypePtr,
        SQLLEN *        LengthPtr,
        SQLSMALLINT *   PrecisionPtr,
        SQLSMALLINT *   ScalePtr,
        SQLSMALLINT *   NullablePtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}






SQLAPI SQLSetDescRec(
        SQLHDESC      DescriptorHandle,
        SQLSMALLINT   RecNumber,
        SQLSMALLINT   Type,
        SQLSMALLINT   SubType,
        SQLLEN        Length,
        SQLSMALLINT   Precision,
        SQLSMALLINT   Scale,
        SQLPOINTER    DataPtr,
        SQLLEN *      StringLengthPtr,
        SQLLEN *      IndicatorPtr)
{
	SQLAPI_DEBUG
    //FIXME: IMPLEMENT
    return SQL_ERROR;
}



