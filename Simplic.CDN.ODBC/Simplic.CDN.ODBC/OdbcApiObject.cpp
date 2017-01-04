#include "stdafx.h"
#include "OdbcApiObject.h"


void odbcHandleException(const std::exception& ex, OdbcApiObject* obj)
{
	if (obj == NULL) return;
	std::string message = "Unhandled exception: ";
	message += ex.what();
	obj->diagInfo()->setRecord(new DiagRecord(message));
}

OdbcApiObject::OdbcApiObject()
{
}


OdbcApiObject::~OdbcApiObject()
{
}
