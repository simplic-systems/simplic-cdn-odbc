// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// macro to create a scoped lock, used in classes accessible from the ODBC API.
#define LOCK(mutex) std::lock_guard<std::recursive_mutex> lock(mutex)

#define VENDOR_IDENTIFIER "EDV-Systeme Spiegelburg"

