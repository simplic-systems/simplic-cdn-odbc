#pragma once
#include "stdafx.h"

extern const char* ODBC_DRIVER_NAME;

class GlobalInfo
{
private:

	static GlobalInfo* _singletonInstance;
	CRITICAL_SECTION m_csGlobal;

	HINSTANCE m_hInstance; // dll handle needed for dialog boxes
	GlobalInfo(HINSTANCE hInstance);
	~GlobalInfo();


public:
	/// Returns a pointer to the singleton instance, or null if it does not exist yet.
	static inline GlobalInfo* getInstance() { return _singletonInstance; }

	/// Creates the singleton instance. Needs to be done in the DllMain when loading the DLL.
	static void createSingletonInstance(HINSTANCE hInstance);

	/// Deletes the singleton instance. Needs to be done before unloading the DLL.
	static void deleteSingletonInstance();



	/// Returns the HINSTANCE of this dll
	inline HINSTANCE getHInstance() { return m_hInstance; }


};