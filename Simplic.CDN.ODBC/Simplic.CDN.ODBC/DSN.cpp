#include "stdafx.h"
#include "dsn.h"

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
	// parsed values - don't use the class members directly so that
	// they don't get changed if the supplied connection string is invalid.
	// The connection string should look like "url=http://127.0.0.1/api;uid=user42;pwd=password1234"
	// TODO check if this is actually needed, if yes, merge with fromNullDelimitedAttributes in order to avoid duplicate code.
	std::string name, url, user, password;
	bool nameValid = false, urlValid = false, userValid = false, passwordValid = false;

	std::stringstream ssConnstr(connstr);
	std::string keyValuePair;
	while (std::getline(ssConnstr, keyValuePair, ';')) {
		// keyValuePair now contains a single key-value pair like "pwd=1234"
		size_t idx = keyValuePair.find('=');
		if (idx == std::string::npos) return false; // no '=' in a key-value pair => invalid connection string
		std::string key = keyValuePair.substr(0, idx);
		std::string value = keyValuePair.substr(idx+1);

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
			if (urlValid) return false; // abort if there are multiple urls in the connection string
			url = value;
			urlValid = true;
		}
		else if (key.compare("uid") == 0)
		{
			if (userValid) return false; // abort if there are multiple urls in the connection string
			user = value;
			userValid = true;
		}
		else if (key.compare("pwd") == 0)
		{
			if (passwordValid) return false; // abort if there are multiple urls in the connection string
			password = value;
			passwordValid = true;
		}
		
	}

	if (nameValid && urlValid && userValid && passwordValid)
	{
		// Valid connection string with userid and password
		// => store all four values
		m_name = name;
		m_url = url;
		m_user = user;
		m_password = password;
		return true;
	}
	else if (nameValid && urlValid && !userValid && !passwordValid)
	{
		// Valid connection string without userid and password
		// => just store the name and the url
		m_name = name;
		m_url = url;
		m_user = "";
		m_password = "";
		return true;
	}
	else
	{
		// Invalid connection string => Keep the old values and return false
		return false;
	}
}


bool DSN::fromNullDelimitedAttributes(const char* attributes)
{
	std::string name, url, user, password;
	bool nameValid = false, urlValid = false, userValid = false, passwordValid = false;

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
			if (urlValid) return false; // abort if there are multiple urls in the connection string
			url = value;
			urlValid = true;
		}
		else if (key.compare("uid") == 0)
		{
			if (userValid) return false; // abort if there are multiple urls in the connection string
			user = value;
			userValid = true;
		}
		else if (key.compare("pwd") == 0)
		{
			if (passwordValid) return false; // abort if there are multiple urls in the connection string
			password = value;
			passwordValid = true;
		}

	}

	if (nameValid && urlValid && userValid && passwordValid)
	{
		// Valid connection string with userid and password
		// => store all four values
		m_name = name;
		m_url = url;
		m_user = user;
		m_password = password;
		return true;
	}
	else if (nameValid && urlValid && !userValid && !passwordValid)
	{
		// Valid connection string without userid and password
		// => just store the name and the url
		m_name = name;
		m_url = url;
		m_user = "";
		m_password = "";
		return true;
	}
	else
	{
		// Invalid connection string => Keep the old values and return false
		return false;
	}
}

