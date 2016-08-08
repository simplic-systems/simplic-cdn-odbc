#pragma once

#include "odbc_api.h"

#include <string>
#include <map>
#include <functional>

class OdbcInfoField
{
public:
	virtual ~OdbcInfoField() = default;

	virtual bool fromOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength) = 0;

	virtual bool toOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength,
		SQLSMALLINT *   dataLengthPtr) = 0;

	virtual OdbcInfoField* clone() = 0;
};

template<typename T>
class DynamicInfoField : public OdbcInfoField
{
protected:
	std::function<T()> m_dynamicValue;
public:
	DynamicInfoField() = default;
	DynamicInfoField(T value);
	DynamicInfoField(std::function<T()> dynamicValue);

	virtual ~DynamicInfoField<T>() = default;

	virtual bool fromOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength) = 0;

	virtual bool toOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength,
		SQLSMALLINT *   dataLengthPtr) = 0;

	virtual OdbcInfoField* clone() = 0;


	T getValue() { return m_dynamicValue(); }
	void setValue(T value) { m_dynamicValue = [=]() {return value; }; }
	void setValue(const std::function<T()>& value) { m_dynamicValue = value; }
};


template<typename T>
class FixedSizeInfoField : public DynamicInfoField<T>
{
public:
	FixedSizeInfoField() = default;
	FixedSizeInfoField(T value);
	FixedSizeInfoField(std::function<T()> dynamicValue);

	virtual bool fromOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength);

	virtual bool toOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength,
		SQLSMALLINT *   dataLengthPtr);

	virtual OdbcInfoField* clone();
};


class StringInfoField : public DynamicInfoField<std::string>
{
public:
	StringInfoField() = default;
	StringInfoField(std::string value);
	StringInfoField(std::function<std::string()> dynamicValue);
	virtual bool fromOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength);

	virtual bool toOdbc(
		SQLPOINTER      buffer,
		SQLSMALLINT     bufferLength,
		SQLSMALLINT *   dataLengthPtr);

	virtual OdbcInfoField* clone();
};





class InfoRecord
{
private:
	std::map<int, OdbcInfoField*> m_fields;

public:
	InfoRecord();
	~InfoRecord();
	OdbcInfoField* getField(int key);
	void addField(int key, OdbcInfoField* field);
};



#include "Info.hpp"