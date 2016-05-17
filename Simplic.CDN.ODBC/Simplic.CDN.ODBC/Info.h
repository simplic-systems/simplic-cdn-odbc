#pragma once

#include "odbc_api.h"

#include <string>
#include <map>

class InfoRecord;
class StringInfoRecord;
class USmallIntInfoRecord;
class UIntInfoRecord;


/// Abstract class representing a single record
/// retrievable by SQLGetInfo()
class InfoRecord
{
public:

	virtual ~InfoRecord();

	/// Gets the information stored in this record.
	/// Compatible with SQLGetInfo() API
	virtual bool getInfo(
		SQLHDBC         ConnectionHandle,
		SQLPOINTER      InfoValuePtr,
		SQLSMALLINT     BufferLength,
		SQLSMALLINT *   StringLengthPtr) const = 0;

	virtual InfoRecord* clone() const = 0;
};

class StringInfoRecord : public InfoRecord
{
private:
	std::string m_value;
public:
	StringInfoRecord(std::string value);
	virtual bool  getInfo(
		SQLHDBC         ConnectionHandle,
		SQLPOINTER      InfoValuePtr,
		SQLSMALLINT     BufferLength,
		SQLSMALLINT *   StringLengthPtr) const;

	virtual InfoRecord* clone() const;
};


class USmallIntInfoRecord : public InfoRecord
{
private:
	SQLUSMALLINT m_value;
public:
	USmallIntInfoRecord(SQLUSMALLINT value) ;
	virtual bool  getInfo(
		SQLHDBC         ConnectionHandle,
		SQLPOINTER      InfoValuePtr,
		SQLSMALLINT     BufferLength,
		SQLSMALLINT *   StringLengthPtr) const;

	virtual InfoRecord* clone() const;
};


class UIntInfoRecord : public InfoRecord
{
private:
	SQLUINTEGER m_value;
public:
	UIntInfoRecord(SQLUINTEGER value);
	virtual bool  getInfo(
		SQLHDBC         ConnectionHandle,
		SQLPOINTER      InfoValuePtr,
		SQLSMALLINT     BufferLength,
		SQLSMALLINT *   StringLengthPtr) const;

	virtual InfoRecord* clone() const;
};




class Info
{
private:
	std::map<SQLUSMALLINT, InfoRecord*> m_records; // maps InfoTypes to their values.
public:
	Info();
	~Info();

	void addRecord(SQLUSMALLINT infoType, const InfoRecord& rec);
	
	bool  getInfo(
		SQLHDBC         ConnectionHandle,
		SQLUSMALLINT    InfoType,
		SQLPOINTER      InfoValuePtr,
		SQLSMALLINT     BufferLength,
		SQLSMALLINT *   StringLengthPtr) const;
};

