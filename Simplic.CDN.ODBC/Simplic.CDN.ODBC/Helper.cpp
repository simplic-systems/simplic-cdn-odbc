#include "stdafx.h"
#include "Helper.h"

std::string Helper::stringFromOdbc(char * strIn, long length)
{
	// ODBC is weird like this...
	if(strIn == NULL) return std::string();
	if (length < 0) return std::string(strIn);
	else return std::string(strIn, length);
}

void Helper::stringToOdbc(std::string strIn, char* buf, uint16_t buflength, uint16_t * lengthPtr)
{
	uint16_t copyLength = (uint16_t)min(size_t(buflength), strIn.size() + 1);
	uint16_t totalLength = (uint16_t)min(0xFFFE, strIn.size());

	if (buf != NULL)
	{
		memcpy(buf, strIn.c_str(), copyLength);
		if (copyLength == buflength && buflength != 0) buf[buflength - 1] = '\0';
	}

	if (lengthPtr != NULL) *lengthPtr = totalLength;
}
