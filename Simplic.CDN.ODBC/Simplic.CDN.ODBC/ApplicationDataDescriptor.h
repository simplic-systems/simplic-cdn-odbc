#pragma once

#include <vector>

class ApplicationDataDescriptorItem;

/**
 * The class ApplicationDataDescriptor contains information about how the application wants us to deliver the result data.
 * This includes information on buffers bound by the application to result columns or parameters.
 * It corresponds to the "ARD" and "APD" described in the ODBC doocumentation: https://msdn.microsoft.com/en-us/library/ms716339(v=vs.85).aspx
 */
class ApplicationDataDescriptor
{
	// general information


	// information about each bound column / parameter
	std::vector<ApplicationDataDescriptorItem> m_items;
};



class ApplicationDataDescriptorItem
{

};