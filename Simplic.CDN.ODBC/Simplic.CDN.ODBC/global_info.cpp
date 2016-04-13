#include "stdafx.h"
#include "global_info.h"

const char* ODBC_DRIVER_NAME = "Simplic.CDN.ODBC Driver";

GlobalInfo* GlobalInfo::_singletonInstance = NULL;

GlobalInfo::GlobalInfo(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	InitializeCriticalSection(&m_csGlobal);
}

GlobalInfo::~GlobalInfo()
{
	DeleteCriticalSection(&m_csGlobal);
}

void GlobalInfo::createSingletonInstance(HINSTANCE hInstance)
{
	_singletonInstance = new GlobalInfo(hInstance);
}


void GlobalInfo::deleteSingletonInstance()
{
	if (_singletonInstance != NULL) delete _singletonInstance;
}