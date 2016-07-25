#include "stdafx.h"
#include "util.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#ifdef _DEBUG

void odbc_api_called(const char* file, int line, const char* function)
{
	// TODO ugly debug code! replace this with some proper logging!
	std::string strTime;
	{
		time_t rawtime;
		struct tm timeinfo;
		char buf[40];

		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		strftime(buf, 40, "[%d-%m-%Y %H:%M:%S]", &timeinfo);
		strTime = buf;
	}


	std::ofstream log;
	log.open("C:\\log\\Simplic.CDN.ODBC.Debug.log", std::ios::out | std::ios::app);

	log << strTime << " " << file << ":" << line << " @ " << function << std::endl;
	
}

#endif


Util::Util() {}

bool Util::stringToOdbcBuffer(std::string value, SQLPOINTER buffer, SQLSMALLINT BufferLength, SQLSMALLINT * StringLengthPtr)
{
	if(StringLengthPtr != NULL) *StringLengthPtr = (SQLSMALLINT)value.size();
	SQLSMALLINT bytesToCopy = min(SQLSMALLINT(value.size()) + 1, BufferLength);
	memcpy(buffer, value.c_str(), bytesToCopy);

	return bytesToCopy <= BufferLength;
}
