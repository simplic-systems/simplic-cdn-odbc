#ifndef ODBC_API_H
#define ODBC_API_H


#define ODBCVER	0x0300
#include <sqltypes.h>
#include <sql.h>
#include <odbcinst.h>

#include <exception>

typedef signed short SQLRETURN;
#define ODBCEXPORT(type) extern "C" type __stdcall
#define SQLAPI ODBCEXPORT(SQLRETURN)

#endif // ODBC_API_H
