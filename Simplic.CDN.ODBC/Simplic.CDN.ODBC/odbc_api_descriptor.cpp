#include "stdafx.h"
#include "odbc_api.h"

SQLAPI SQLCopyDesc(
        SQLHDESC     SourceDescHandle,
        SQLHDESC     TargetDescHandle)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLGetDescField(
        SQLHDESC        DescriptorHandle,
        SQLSMALLINT     RecNumber,
        SQLSMALLINT     FieldIdentifier,
        SQLPOINTER      ValuePtr,
        SQLINTEGER      BufferLength,
        SQLINTEGER *    StringLengthPtr)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}






SQLAPI SQLSetDescField(
        SQLHDESC      DescriptorHandle,
        SQLSMALLINT   RecNumber,
        SQLSMALLINT   FieldIdentifier,
        SQLPOINTER    ValuePtr,
        SQLINTEGER    BufferLength)
{
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
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
    //FIXME: IMPLEMENT
    return SQL_SUCCESS;
}



