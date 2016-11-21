#include "stdafx.h"
#include "util.h"
#include "global_info.h"
#include "odbc_api.h"
#include "OdbcApiObject.h"
#include "DSN.h"



#define ODBCSETUP BOOL __stdcall

ODBCSETUP ConfigDriver(
      HWND    hwndParent,
      WORD    fRequest,
      LPCSTR  lpszDriver,
      LPCSTR  lpszArgs,
      LPSTR   lpszMsg,
      WORD    cbMsgMax,
      WORD *  pcbMsgOut)
{
	SQLAPI_DEBUG
    switch(fRequest)
    {
    case ODBC_INSTALL_DRIVER:
        // TODO
        return TRUE;

    case ODBC_REMOVE_DRIVER:
        // TODO
        return TRUE;

    default:
        return FALSE;
    }
}


ODBCSETUP ConfigDSN(
     HWND     hwndParent,
     WORD     fRequest,
     LPCSTR   lpszDriver,
     LPCSTR   lpszAttributes)
{
	SQLAPI_DEBUG;
	try
	{
		DSN dsnPredefined, dsnNew;
		// lpszAttributes contains the parameters of our connection string, delimited by null characters.
		bool validPredefinedDSN = dsnPredefined.fromNullDelimitedAttributes(lpszAttributes);
		dsnNew = dsnPredefined;

		// Note: If hwndParent == NULL, we are supposed not to display any kind of dialog boxes.


		if ((fRequest == ODBC_CONFIG_DSN || fRequest == ODBC_REMOVE_DSN) && !validPredefinedDSN)
		{
			if (hwndParent)
				MessageBoxA(hwndParent, "Ungültiges DSN-Format", "Simplic.CDN.ODBC", MB_ICONERROR);
			return FALSE;
		}

		if (fRequest == ODBC_REMOVE_DSN) // remove?
		{
			SQLRemoveDSNFromIni(dsnPredefined.getName().c_str());
			return TRUE;
		}


		// if we are editing an existing DSN, load its attributes from the registry
		// if not, use the values defined in dsnPredefined. (if these are not specified, they default to the empty string).
		if (fRequest == ODBC_CONFIG_DSN)
		{
			dsnPredefined.loadAttributesFromRegistry();
		}

		if (hwndParent)
		{
			dsnNew = dsnPredefined;
			bool accepted = dsnNew.showConfigDialog(hwndParent);
			if (!accepted) return TRUE; // abort if the user cancelled the dialog
		}


		// Create a new DSN if we are creating a new one from scratch or changing the name of an existing one
		if (fRequest == ODBC_ADD_DSN || !dsnPredefined.equalName(dsnNew))
		{
			if (!SQLWriteDSNToIni(dsnNew.getName().c_str(), ODBC_DRIVER_NAME))
			{
				if (hwndParent) MessageBoxA(hwndParent, "DSN konnte nicht erstellt werden.", "Simplic.CDN.ODBC", MB_ICONERROR);
				return FALSE;
			}
		}

		// Write attributes to the registry
		BOOL success = TRUE;
		const char* filename = "ODBC.INI";
		success = success && SQLWritePrivateProfileString(dsnNew.getName().c_str(), "url", dsnNew.getUrl().c_str(), filename);
		success = success && SQLWritePrivateProfileString(dsnNew.getName().c_str(), "uid", dsnNew.getUser().c_str(), filename);
		success = success && SQLWritePrivateProfileString(dsnNew.getName().c_str(), "pwd", dsnNew.getPassword().c_str(), filename);
		if (!success)
		{
			if (hwndParent) MessageBoxA(hwndParent, "Fehler beim Speichern der DSN-Attribute", "Simplic.CDN.ODBC", MB_ICONERROR);
			// Delete the faulty DSN if we just created it from scratch
			if (fRequest == ODBC_ADD_DSN || !dsnPredefined.equalName(dsnNew)) SQLRemoveDSNFromIni(dsnNew.getName().c_str());
			return FALSE;
		}

		// Delete old DSN from registry if we changed the name of an existing DSN.
		if (fRequest == ODBC_CONFIG_DSN && !dsnPredefined.equalName(dsnNew))
		{
			SQLRemoveDSNFromIni(dsnPredefined.getName().c_str());
		}


		return TRUE;
	}
	catch (const std::exception& ex) { odbcHandleException(ex, NULL); return FALSE; }
}



ODBCSETUP ConfigTranslator(
     HWND     hwndParent,
     DWORD *  pvOption)
{
	SQLAPI_DEBUG
	// FIXME: IMPLEMENT
	
    *pvOption = 0;
    return TRUE;
}
