#include "stdafx.h"
#include "global_info.h"
#include "sqlext.h"

const char* ODBC_DRIVER_NAME = "Simplic.CDN.ODBC ANSI";

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
	m_info.addRecord(SQL_DRIVER_ODBC_VER, StringInfoRecord("03.00"));
}

void GlobalInfo::createSingletonInstance(HINSTANCE hInstance)
{
	_singletonInstance = new GlobalInfo(hInstance);
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

Info * GlobalInfo::getInfo()
{
	if (!m_info_initialized) initializeInfo();
	return &m_info;
}
