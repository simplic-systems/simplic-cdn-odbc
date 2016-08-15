#include "stdafx.h"
#include "DbConnection.h"
#include "Statement.h"

#include "json/json.h"


DbConnection::DbConnection(Environment * env)
	: m_timeout(10)
{
	m_environment = env;
}

DbConnection::~DbConnection()
{
	LOCK(m_mutex);
	for (Statement* stmt : m_statements) delete stmt;
	m_statements.clear();
	m_environment->removeConnection(this);
}

Statement * DbConnection::createStatement()
{
	LOCK(m_mutex);
	Statement* stmt = new Statement(this);
	m_statements.insert(stmt);
	return stmt;
}

void DbConnection::removeStatement(Statement * stmt)
{
	LOCK(m_mutex);
	m_statements.erase(stmt);
}

void DbConnection::setTimeout(uint32_t timeout)
{
	m_timeout = timeout;
}

bool DbConnection::connect(std::string url, std::string user, std::string password)
{
	if (url.size() < 1) return false;
	if (url[url.size() - 1] != '/') url = url + "/"; // add trailing slash to url if it's not there yet
	url = url + "api/v1-0/";

	// TODO:
	// HTTP POST url + "auth/login" 
	// if(HTTP Status not OK) return false;

	// TODO: Remove this dummy!
	if (user == std::string("invalid")) return false;

	// the response will look like this dummy response
	std::string responseString = "{ \"token\": \"dummy.auth.token\" }";
	Json::Reader reader;
	Json::Value response;
	if (!reader.parse(responseString, response, false)) // parse responseString into response, skipping comments
	{
		return false;
	}

	
	m_url = url;
	m_authToken = "DummyAuthToken"; // we will get this from the auth api call
	return true;
}

Json::Value* DbConnection::executeCommand(const std::string & command, const Json::Value & parameters)
{
	// TODO:
	// use command in url: m_url + "odbc/" + command
	// use m_authToken in HTTP 'Authorization' header: "jwt " + m_authToken
	// use json-decoded parameters as content.

	// TODO:
	// decode the result as a json object
	// extract status information from HTTP status

	// dummy response:
	std::string responseString =
		"{"
		"    \"meta\" : {"
		"        \"rowcount\" : 2 ,"
		"        \"columns\" : ["
		"            { \"name\" : \"TABLE_CAT\" , \"type\" : 12 }," // type 12 is SQL_VARCHAR
		"            { \"name\" : \"TABLE_SCHEM\" , \"type\" : 12 },"
		"            { \"name\" : \"TABLE_NAME\" , \"type\" : 12 },"
		"            { \"name\" : \"TABLE_TYPE\" , \"type\" : 12 },"
		"            { \"name\" : \"TABLE_REMARKS\" , \"type\" : 12 }"
		"        ]"
		"    },"
		"    "
		"    \"rows\" : ["
		"        [null,null,\"firsttable\",\"TABLE\",\"dummy\"],"
		"        [null,null,\"secondtable\",\"TABLE\",\"dummy\"]"
		"    ]"
		"}";

	Json::Reader reader;

	if (!reader.parse(responseString, m_apiResult, false)) // parse responseString into response, skipping comments
	{
		m_apiResult = Json::Value(); // clear result object if parsing failed
	}

	return &m_apiResult;
}

bool DbConnection::fetch(std::vector<Json::Value*> & result, uint32_t fromRow, uint32_t numRows)
{
	result.clear();
	Json::Value& rows = m_apiResult["rows"];
	if (rows.isNull()) return false;

	uint32_t rowsAvailable = rows.size();

	for (uint32_t i = fromRow; i < fromRow + numRows && i < rowsAvailable; ++i)
	{
		result.push_back(&rows[i]);
	}

	return true;
}
