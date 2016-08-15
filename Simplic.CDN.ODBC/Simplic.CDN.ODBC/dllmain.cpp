// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "global_info.h"
#include "OdbcTypeConverter.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GlobalInfo::createSingletonInstance(hModule);
		OdbcTypeConverter::createSingletonInstance();
		break;

	case DLL_PROCESS_DETACH:
		GlobalInfo::deleteSingletonInstance();
		OdbcTypeConverter::deleteSingletonInstance();
		break;
	}
	return TRUE;
}

