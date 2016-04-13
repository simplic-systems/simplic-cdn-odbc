#pragma once
#include "stdafx.h"
#include "DSN.h"

class ConfigDSNDialog
{
private:
	/// DSN information entered in the dialog.
	/// This will only update when the user confirms the dialog.
	DSN m_dsn;

	/// Callback that handles messages from the dialog window.
	static INT_PTR CALLBACK DlgMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	ConfigDSNDialog();

	// Shows the modal dialog and returns true iff the DSN was set successfully by the user.
	// If the user does not cancel the dialog, the DSN stored by this class will be populated
	// with the data entered by the user.
	// The parameter dsnCurrent can be used to specify default values for the edit fields.
	bool showDialog(HINSTANCE hInstance, HWND hwndParent, const DSN* dsnCurrent = NULL);

	DSN getDSN() const;
	void setDSN(const DSN& dsn);

};