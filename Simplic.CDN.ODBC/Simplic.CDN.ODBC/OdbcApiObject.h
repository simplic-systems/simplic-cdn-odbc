#pragma once
#include "DiagInfo.h"

// Class for all objects that interface directly with the ODBC API.
class OdbcApiObject
{
protected:
	DiagInfo m_diagInfo;

public:
	OdbcApiObject();
	virtual ~OdbcApiObject();

	inline DiagInfo* diagInfo() { return &m_diagInfo; }
};

