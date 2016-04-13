#include "stdafx.h"
#include "global_info.h"
#include "odbc_api.h"
#include "ui_ConfigDSNDialog.h"


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
    switch(fRequest)
    {
    case ODBC_INSTALL_DRIVER:
        // FIXME DEBUG


		MessageBoxA(hwndParent,
			"ConfigDriver: Installing Driver",
			"Debug Message", MB_OK);
        return TRUE;

    case ODBC_REMOVE_DRIVER:
        // FIXME DEBUG
		MessageBoxA(hwndParent,
			"ConfigDriver: Removing Driver",
			"Debug Message", MB_OK);
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
    // lpszAttributes contains the parameters of our connection string, delimited by null characters.
	//
	DSN dsnPredefined;
	bool validPredefinedDSN = dsnPredefined.fromNullDelimitedAttributes(lpszAttributes);

	//FIXME: This function should not display any dialog boxes if hwndParent is null!
	ConfigDSNDialog dialog;
	if (dialog.showDialog(GlobalInfo::getInstance()->getHInstance(), hwndParent, validPredefinedDSN ? (&dsnPredefined) : NULL))
	{
		DSN dsnNew = dialog.getDSN();

		// Determine if we are editing an existing DSN or if we are creating a new one.
		bool editingExistingDSN = validPredefinedDSN && dsnPredefined.equalName(dsnNew);
		if (!SQLWriteDSNToIni(dsnNew.getName().c_str(), ODBC_DRIVER_NAME))
		{
			MessageBoxA(hwndParent, "DSN konnte nicht erstellt werden.", "Simplic.CDN.ODBC", MB_ICONERROR);
			MessageBoxA(hwndParent, dsnNew.getName().c_str() , "Simplic.CDN.ODBC", MB_ICONERROR); // FIXME DEBUG
			return FALSE;
		}

		// Write attributes
		// TODO need to write the path to the dll into the "driver" key so that the driver manager
		// can find the dll for deleting and configuring existing DSNs.
		BOOL success = TRUE;
		const char* filename = "ODBC.INI";
		success = success && SQLWritePrivateProfileString(dsnNew.getName().c_str(), "url", dsnNew.getUrl().c_str(), filename);
		success = success && SQLWritePrivateProfileString(dsnNew.getName().c_str(), "uid", dsnNew.getUser().c_str(), filename);
		success = success && SQLWritePrivateProfileString(dsnNew.getName().c_str(), "pwd", dsnNew.getPassword().c_str(), filename);
		if (!success)
		{
			MessageBoxA(hwndParent, "Fehler beim Speichern der DSN-Attribute", "Simplic.CDN.ODBC", MB_ICONERROR);
			// Delete the faulty DSN if we just created it from scratch
			if(!editingExistingDSN) SQLRemoveDSNFromIni(dsnNew.getName().c_str());
			return FALSE;
		}

		// Delete old DSN from registry if we changed the name of an existing DSN.
		if (validPredefinedDSN && !editingExistingDSN)
		{
			SQLRemoveDSNFromIni(dsnPredefined.getName().c_str());
		}
	}

    return TRUE;
}



ODBCSETUP ConfigTranslator(
     HWND     hwndParent,
     DWORD *  pvOption)
{
	// FIXME DEBUG
	
    *pvOption = 0;
    return TRUE;
}
