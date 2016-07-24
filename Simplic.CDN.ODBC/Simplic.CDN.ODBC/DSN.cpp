#include "stdafx.h"
#include "dsn.h"
#include "odbc_api.h"
#include "global_info.h"
#include "ui_ConfigDSNDialog.h"

#include <sstream>
#include <algorithm>

//TODO consider moving this to some util class
void DSN::stringToLowercase(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](char x) { return ::tolower((unsigned char)x); });
}

DSN::DSN()
{

}

DSN::DSN(std::string url, std::string user, std::string password)
{
	m_url = url;
	m_user = user;
	m_password = password;
}



std::string DSN::getName() const
{
	return m_name;
}

void DSN::setName(std::string name)
{
	m_name = name;
}

std::string DSN::getUrl() const
{
	return m_url;
}

void DSN::setUrl(std::string url)
{
	m_url = url;
}

std::string DSN::getUser() const
{
	return m_user;
}

void DSN::setUser(std::string user)
{
	m_user = user;
}


std::string DSN::getPassword() const
{
	return m_password;
}

void DSN::setPassword(std::string password)
{
	m_password = password;
}

std::string DSN::toConnectionString() const
{
	std::string result = std::string("DSN=") + m_name + ";URL=" + m_url;

	// append "uid" and "pwd" only if user id is not empty.
	if (m_user.size() > 0)
	{
		result += std::string(";Uid=") + m_user + ";Pwd=" + m_password;
	}
	return result;
}


bool DSN::equalName(const DSN& other) const
{
	std::string myName(m_name), otherName(other.getName());
	stringToLowercase(myName);
	stringToLowercase(otherName);
	return myName.compare(otherName) == 0;
}

bool DSN::fromConnectionString(std::string connstr)
{
	// add trailing ';' if it does not exist
	if (connstr[connstr.size() - 1] != ';') connstr.append(";");

	// prevent overflow
	if (connstr.size() > 65535) return false;

	char buf[65536];
	strcpy_s(buf, connstr.c_str());
	// replace all ';' with null characters
	for (int i = 0; i < connstr.size(); ++i)
		if (buf[i] == ';') buf[i] = '\0';

	// Now we can parse the string as "null delimited attribute string"
	return fromNullDelimitedAttributes(buf);
}

void DSN::loadAttributesFromRegistry()
{
	char buf[65536];
	const char* filename = "ODBC.INI";
	if (getName().size() == 0) return; // don't load from registry if this dsn does not have a name.
	SQLGetPrivateProfileString(getName().c_str(), "url", getUrl().c_str(), buf, 65536, filename);
	setUrl(buf);
	SQLGetPrivateProfileString(getName().c_str(), "uid", getUser().c_str(), buf, 65536, filename);
	setUser(buf);
	SQLGetPrivateProfileString(getName().c_str(), "pwd", getPassword().c_str(), buf, 65536, filename);
	setPassword(buf);
}

bool DSN::showConfigDialog(HWND hwndParent)
{
	ConfigDSNDialog dialog;
	bool accepted = dialog.showDialog(GlobalInfo::getInstance()->getHInstance(), hwndParent, this);
	if (accepted) (*this) = dialog.getDSN();
	return accepted; // return true iff the user accepted the changes
}


bool DSN::fromNullDelimitedAttributes(const char* attributes)
{
	std::string name, url, user, password;
	bool nameValid = false;

	if (attributes == NULL) return false;

	// attributes looks like this: "DSN=Personnel Data\0URL=some_url\0UID=Smith\0PWD=Sesame\0\0"
	// This loop iterates through all key-value pairs individually by advancing the "attributes" pointer to
	// the beginning of the next key-value pair after each iteration.
	for (std::string keyValuePair(attributes); attributes[0] != 0; attributes += keyValuePair.size() + 1)
	{
		// keyValuePair now contains a single key-value pair like "pwd=1234"
		size_t idx = keyValuePair.find('=');
		if (idx == std::string::npos) return false; // no '=' in a key-value pair => invalid connection string
		std::string key = keyValuePair.substr(0, idx);
		std::string value = keyValuePair.substr(idx + 1);

		// convert key to lowercase for case-insensitive comparison
		stringToLowercase(key);

		if (key.compare("dsn") == 0)
		{
			if (nameValid) return false; // abort if there are multiple names in the connection string
			name = value;
			nameValid = true;
		}
		if (key.compare("url") == 0)
		{
			url = value;
		}
		else if (key.compare("uid") == 0)
		{
			user = value;
		}
		else if (key.compare("pwd") == 0)
		{
			password = value;
		}

	}

	if (nameValid || (url.length() != 0 && user.length() != 0 & password.length() != 0))
	{
		// Valid connection string with userid and password
		// => store all four values
		m_name = name;
		m_url = url;
		m_user = user;
		m_password = password;
		return true;
	}
	return false;
}

