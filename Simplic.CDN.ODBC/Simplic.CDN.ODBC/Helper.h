#pragma once

#include <string>

// class for various helper functions
class Helper
{
private:
	Helper() = default; // static functions only in this class => don't allow instancing it
public:

	/** Converts a C string from ODBC into a std::string. 
	  * If strIn is NULL, an empty string will be returned
	  * If the length parameter is negative, it is ignored and strIn
	  * is interpreted as a null-terminated C string.  */
	static std::string stringFromOdbc(char* strIn, long length);


	static void stringToOdbc(std::string strIn, char* buf, uint16_t buflength, uint16_t *lengthPtr);

	static bool isBinaryType(int16_t type);

};