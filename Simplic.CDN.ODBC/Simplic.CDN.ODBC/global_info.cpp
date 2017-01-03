#include "stdafx.h"
#include "global_info.h"
#include "sqlext.h"

#ifdef _WIN64
const char* ODBC_DRIVER_NAME = "Simplic.CDN.ODBC ANSI x64";
#else
const char* ODBC_DRIVER_NAME = "Simplic.CDN.ODBC ANSI x86";
#endif

GlobalInfo* GlobalInfo::_singletonInstance = NULL;

GlobalInfo::GlobalInfo(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_info_initialized = false;
}

GlobalInfo::~GlobalInfo()
{
}

void GlobalInfo::initializeInfo()
{
	m_info.addField(SQL_DRIVER_NAME, new StringInfoField(TARGET_FILENAME));
	m_info.addField(SQL_DRIVER_ODBC_VER, new StringInfoField("03.00"));

	m_info.addField(SQL_CURSOR_COMMIT_BEHAVIOR, new FixedSizeInfoField<SQLUSMALLINT>(SQL_CB_CLOSE));
	m_info.addField(SQL_CURSOR_ROLLBACK_BEHAVIOR, new FixedSizeInfoField<SQLUSMALLINT>(SQL_CB_CLOSE));
	m_info.addField(SQL_GETDATA_EXTENSIONS, new FixedSizeInfoField<SQLUINTEGER>(0));
	
	m_info.addField(SQL_MAX_CATALOG_NAME_LEN, new FixedSizeInfoField<SQLUSMALLINT>(0));
	m_info.addField(SQL_MAX_SCHEMA_NAME_LEN, new FixedSizeInfoField<SQLUSMALLINT>(0));
	m_info.addField(SQL_TXN_CAPABLE, new FixedSizeInfoField<SQLUSMALLINT>(0));
	m_info.addField(SQL_IDENTIFIER_CASE, new FixedSizeInfoField<SQLUSMALLINT>(SQL_IC_MIXED));
	m_info.addField(SQL_SEARCH_PATTERN_ESCAPE, new StringInfoField(""));
	m_info.addField(SQL_QUALIFIER_NAME_SEPARATOR, new StringInfoField("."));
	m_info.addField(SQL_NON_NULLABLE_COLUMNS, new FixedSizeInfoField<SQLUSMALLINT>(SQL_NNC_NON_NULL));
	m_info.addField(SQL_SCROLL_CONCURRENCY, new FixedSizeInfoField<SQLUINTEGER>(0));
	m_info.addField(SQL_TXN_ISOLATION_OPTION, new FixedSizeInfoField<SQLUINTEGER>(0));

	m_info.addField(SQL_DBMS_VER, new StringInfoField("00.01.1300"));
	m_info.addField(SQL_DBMS_NAME, new StringInfoField("Simplic.CDN"));
}

void GlobalInfo::createSingletonInstance(HINSTANCE hInstance)
{
	_singletonInstance = new GlobalInfo(hInstance); 
	_singletonInstance->initializeInfo();
}


void GlobalInfo::deleteSingletonInstance()
{
	if (_singletonInstance != NULL) delete _singletonInstance;
}

std::string GlobalInfo::getDriverPath() const
{
	char buf[MAX_PATH] = {0};
	GetModuleFileNameA(m_hInstance, buf, MAX_PATH);
	return std::string(buf);
}

OdbcInfoField* GlobalInfo::getInfoField(DbConnection* conn, SQLUSMALLINT type)
{
	// TODO: Support querying information from the DbConnection, 
	// if any is provided to us
	return m_info.getField(type);
}
