#include "stdafx.h"
#include "ui_ConfigDSNDialog.h"
#include "resource.h"

INT_PTR ConfigDSNDialog::DlgMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		// populate the edit fields with the information from the DSN referenced by lParam
		const DSN* dsn = (DSN*)lParam;
		if (dsn != NULL)
		{
			SetDlgItemText(hwnd, IDC_EDIT_NAME, dsn->getName().c_str());
			SetDlgItemText(hwnd, IDC_EDIT_URL, dsn->getUrl().c_str());
			SetDlgItemText(hwnd, IDC_EDIT_USER, dsn->getUser().c_str());
			SetDlgItemText(hwnd, IDC_EDIT_PASSWORD, dsn->getPassword().c_str());
		}
		return FALSE;
	}
	
	case WM_CLOSE:
		EndDialog(hwnd, NULL);
		return TRUE;

	case WM_COMMAND:
		// WM_COMMAND = some action happened like a button click
		// LOWORD(wParam) contains the ID of the control that issued the command.
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			// "OK" button pressed => save the data entered by the user and close the dialog window
			char buf[16384]; // buffer for the strings from the edit boxes, this should be large enough even for long URLs.
			DSN* dsn = new DSN();
			GetDlgItemText(hwnd, IDC_EDIT_NAME, buf, 16384);
			dsn->setName(std::string(buf));
			GetDlgItemText(hwnd, IDC_EDIT_URL, buf, 16384);
			dsn->setUrl(std::string(buf));
			GetDlgItemText(hwnd, IDC_EDIT_USER, buf, 16384);
			dsn->setUser(std::string(buf));
			GetDlgItemText(hwnd, IDC_EDIT_PASSWORD, buf, 16384);
			dsn->setPassword(std::string(buf));

			EndDialog(hwnd, (INT_PTR)dsn);
		}
			break;
		case IDCANCEL:
			// "Cancel" button pressed => close the dialog window
			EndDialog(hwnd, NULL); 
			break;
		}
	}
	
	return FALSE; // always return false if we didn't handle the message
}

ConfigDSNDialog::ConfigDSNDialog()
{
}


bool ConfigDSNDialog::showDialog(HINSTANCE hInstance, HWND hwndParent, const DSN* dsnCurrent)
{
	DSN dsnResult; 

	// show the dialog box - this blocks until the dialog is closed.
	DSN* dialogResult = (DSN*) DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_DIALOG_CONFIGDSN),
		hwndParent,
		&DlgMain,
		NULL);

	// store the DSN we got from the dialog (if any)
	if (dialogResult != NULL)
	{
		m_dsn = *dialogResult;
		delete dialogResult; // dialogResult was allocated in DlgMain so we need to delete it!
	}

	return dialogResult != NULL; // result == NULL means that the user cancelled the dialog
}

DSN ConfigDSNDialog::getDSN() const
{
	return m_dsn;
}

void ConfigDSNDialog::setDSN(const DSN & dsn)
{
	m_dsn = dsn;
}
