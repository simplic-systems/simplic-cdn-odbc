#pragma once

#include <string>

/*
* This class represents a DSN for Simplic.CDN consisting of URL, user name and password.
* It can also convert this information into a string representation and parse those strings.
*/
class DSN
{
private:
	static void stringToLowercase(std::string & str);

	std::string m_name;
	std::string m_url;
	std::string m_user;
	std::string m_password;

public:
	DSN();
	DSN(std::string url, std::string user, std::string password);

	std::string getName() const;
	void setName(std::string name);

	std::string getUrl() const;
	void setUrl(std::string url);

	std::string getUser() const;
	void setUser(std::string user);

	std::string getPassword() const;
	void setPassword(std::string password);

	/// Returns true iff the name of the other DSN equals this DSN's name
	/// The comparison is not case sensitive.
	bool equalName(const DSN& other) const;

	/**
	* Populates this object from the information contained in a string delimited by null bytes, 
	* as passed to the ConfigDSN function.
	* Returns true iff the connection string was parsed successfully.
	*/
	bool fromNullDelimitedAttributes(const char* attributes);

	/**
	* Creates a single-string representation of this DSN.
	*/
	std::string toConnectionString() const;

	/**
	* Populates this object from the information contained in a connection string.
	* Returns true iff the connection string was parsed successfully.
	*/
	bool fromConnectionString(std::string connstr);

};

