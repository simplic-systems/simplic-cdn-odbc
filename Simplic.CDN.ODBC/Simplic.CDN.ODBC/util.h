#pragma once

// this macro allows us to set a single breakpoint that will trigger when any
// ODBC API function is called.
#ifdef _DEBUG


#define SQLAPI_DEBUG odbc_api_called(__FILE__, __LINE__, __FUNCTION__);

void odbc_api_called(const char* file, int line, const char* function);

#else

#define SQLAPI_DEBUG /*nop*/

#endif
