#pragma once

#include "stdafx.h"
#include "Info.h"

template<typename T>
DynamicInfoField<T>::DynamicInfoField(T value)
{
	setValue(value);
}

template<typename T>
DynamicInfoField<T>::DynamicInfoField(std::function<T()> dynamicValue)
{
	setValue(dynamicValue);
}



template<typename T>
FixedSizeInfoField<T>::FixedSizeInfoField(T value)
	: DynamicInfoField<T>(value) {}

template<typename T> 
FixedSizeInfoField<T>::FixedSizeInfoField(std::function<T()> dynamicValue) 
	: DynamicInfoField<T>(dynamicValue) {}

template<typename T>
bool FixedSizeInfoField<T>::fromOdbc(SQLPOINTER buffer, SQLSMALLINT bufferLength)
{
	if (buffer == NULL) return false;
	setValue(*(T*)buffer);
	return true;
}

template<typename T>
bool FixedSizeInfoField<T>::toOdbc(SQLPOINTER buffer, SQLSMALLINT bufferLength, SQLSMALLINT * dataLengthPtr)
{
	// we can ignore BufferLength here and assume that the buffer is large enough
	// (this is allowed for integer values)
	if(dataLengthPtr != NULL) *dataLengthPtr = sizeof(T);
	if (buffer == NULL) return true; // buffer == NULL => ODBC app doesn't want the value
	*(T*)buffer = getValue();
	return true;
}

template<typename T>
bool FixedSizeInfoField<T>::toOdbcSegregated(
	SQLPOINTER      charBuffer,
	SQLLEN *        numericBuffer,
	SQLSMALLINT     bufferLength,
	SQLSMALLINT *   dataLengthPtr)
{
	return toOdbc((SQLPOINTER)numericBuffer, bufferLength, dataLengthPtr);
}

template<typename T>
OdbcInfoField* FixedSizeInfoField<T>::clone()
{
	FixedSizeInfoField<T> *cloned = new FixedSizeInfoField<T>();
	cloned->setValue(m_dynamicValue);
	return cloned;
}



