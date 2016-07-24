#include "stdafx.h"
#include "Info.h"


StringInfoRecord::StringInfoRecord(std::string value)
{
	m_value = value;
	// truncate string, SQLGetInfo can't handle buffers with more than 32767 bytes
	// (we truncate it to 32766 chars to leave space for the terminating \0)
	if(m_value.size() > 32766) m_value.erase(32766); 
}

bool StringInfoRecord::getInfo(SQLHDBC ConnectionHandle, SQLPOINTER InfoValuePtr, SQLSMALLINT BufferLength, SQLSMALLINT * StringLengthPtr) const
{
	*StringLengthPtr = (SQLSMALLINT) m_value.size();
	SQLSMALLINT bytesToCopy = min(SQLSMALLINT(m_value.size()) + 1, BufferLength);
	memcpy(InfoValuePtr, m_value.c_str(), bytesToCopy);

	return bytesToCopy <= BufferLength;
}

InfoRecord * StringInfoRecord::clone() const
{
	return new StringInfoRecord(m_value);
}

USmallIntInfoRecord::USmallIntInfoRecord(SQLUSMALLINT value)
{
	m_value = value;
}

bool USmallIntInfoRecord::getInfo(SQLHDBC ConnectionHandle, SQLPOINTER InfoValuePtr, SQLSMALLINT BufferLength, SQLSMALLINT * StringLengthPtr) const
{
	// we can ignore BufferLength here and assume that the buffer is large enough
	// (this is allowed for integer values)
	*StringLengthPtr = 2;
	*(SQLUSMALLINT*)InfoValuePtr = m_value;
	return true;
}

InfoRecord * USmallIntInfoRecord::clone() const
{
	return new USmallIntInfoRecord(m_value);
}

UIntInfoRecord::UIntInfoRecord(SQLUINTEGER value)
{
	m_value = value;
}

bool UIntInfoRecord::getInfo(SQLHDBC ConnectionHandle, SQLPOINTER InfoValuePtr, SQLSMALLINT BufferLength, SQLSMALLINT * StringLengthPtr) const
{
	// we can ignore BufferLength here and assume that the buffer is large enough
	// (this is allowed for integer values)
	*StringLengthPtr = 4;
	*(SQLUINTEGER*)InfoValuePtr = m_value;
	return true;
}

InfoRecord * UIntInfoRecord::clone() const
{
	return new UIntInfoRecord(m_value);
}



Info::Info()
{
}

Info::~Info()
{
	for (auto pair : m_records)
	{
		delete pair.second;
	}
	m_records.clear();
}

void Info::addRecord(SQLUSMALLINT infoType, const InfoRecord & rec)
{
	m_records[infoType] = rec.clone();
}

bool Info::getInfo(SQLHDBC ConnectionHandle, SQLUSMALLINT InfoType, SQLPOINTER InfoValuePtr, SQLSMALLINT BufferLength, SQLSMALLINT * StringLengthPtr) const
{
	auto it = m_records.find(InfoType);
	if (it == m_records.end())
	{
		// No record found for this InfoType? => fail
		if(StringLengthPtr != NULL) *StringLengthPtr = 0;
		return false;
	}
	else
	{
		// Found a record? => let the record object take care of this
		return it->second->getInfo(ConnectionHandle, InfoValuePtr, BufferLength, StringLengthPtr);
	}
	
}

InfoRecord::~InfoRecord()
{
}
