#pragma once
#include "stdafx.h"
#include "Info.h"
#include <string>
#include <mutex>

extern const char* ODBC_DRIVER_NAME;

class Environment;
class DbConnection;

class GlobalInfo
{
private:

	static GlobalInfo* _singletonInstance;
	std::recursive_mutex m_mutex;

	HINSTANCE m_hInstance; // dll handle needed for dialog boxes
	InfoRecord m_info; // information retrievable by SQLGetInfo
	bool m_info_initialized; // true if m_info is populated with data.

	GlobalInfo(HINSTANCE hInstance);
	~GlobalInfo();

	void initializeInfo();


public:
	/// Returns a pointer to the singleton instance, or null if it does not exist yet.
	static inline GlobalInfo* getInstance() { return _singletonInstance; }

	/// Creates the singleton instance. Needs to be done in the DllMain when loading the DLL.
	static void createSingletonInstance(HINSTANCE hInstance);

	/// Deletes the singleton instance. Needs to be done before unloading the DLL.
	static void deleteSingletonInstance();

	/// Returns the HINSTANCE of this dll
	inline HINSTANCE getHInstance() const { return m_hInstance; }

	/// Returns the path of this dll
	std::string getDriverPath() const;

	OdbcInfoField* getInfoField(DbConnection* conn, SQLUSMALLINT type);


};