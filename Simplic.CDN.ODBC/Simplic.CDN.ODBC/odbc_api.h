#ifndef ODBC_H
#define ODBC_H


#define ODBCVER	0x0300
#include <sqltypes.h>
#include <sql.h>
#include <odbcinst.h>

typedef signed short SQLRETURN;
#define ODBCEXPORT(type) extern "C" type __stdcall
#define SQLAPI ODBCEXPORT(SQLRETURN)




#endif // ODBC_H
