#include "stdafx.h"
#include "DbConnection.h"
#include "Statement.h"

#include "json/json.h"

/// Function called by CURL when data has been received.
static size_t ReceiveJson(void *contents, size_t size, size_t nmemb, void *userp)
{
	return ((DbConnection*)userp)->receiveJson(contents, size*nmemb);
}


DbConnection::DbConnection(Environment * env)
	: m_timeout(10)
{
	m_environment = env;
	m_curl = curl_easy_init();
	m_headers = NULL;
}

DbConnection::~DbConnection()
{
	LOCK(m_mutex);
	for (Statement* stmt : m_statements) delete stmt;
	m_statements.clear();

	curl_easy_cleanup(m_curl);
	if (m_headers != NULL) curl_slist_free_all(m_headers);

	m_environment->removeConnection(this);
}

size_t DbConnection::receiveJson(void *contents, size_t size)
{
	m_recvbufJson.write((char*)contents, size);
	return size; // if we return less than size, curl will assume that there's an error.
}

std::string DbConnection::encodeGetParameters(const Json::Value& parameters)
{
	std::stringstream ss;
	char separator = '?';
	for (Json::ValueConstIterator it = parameters.begin(); it != parameters.end(); ++it)
	{
		// key
		char* escaped = curl_easy_escape(m_curl, it.key().asCString(), 0);
		ss << separator << escaped;
		curl_free(escaped);

		// value
		escaped = curl_easy_escape(m_curl, it->asCString(), 0);
		ss << "=" << escaped;
		curl_free(escaped);

		separator = '&'; // use '&' to separate key-value pairs
	}
	return ss.str();
}

void DbConnection::curlReset()
{
	curl_easy_reset(m_curl);
	if (m_headers != NULL) curl_slist_free_all(m_headers);
	m_headers = NULL;
}

void DbConnection::curlPrepareReceiveJson()
{
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, ReceiveJson);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)this);
}

void DbConnection::curlPrepareAuth()
{
	m_headers = curl_slist_append(m_headers, (std::string("Authorization: jwt ") + m_authToken).c_str());
}

void DbConnection::curlPrepareGet(std::string endpoint, const Json::Value& parameters)
{
	std::string parameterString = encodeGetParameters(parameters);
	curl_easy_setopt(m_curl, CURLOPT_URL, (m_url + endpoint + parameterString).c_str());
}

void DbConnection::curlPreparePost(std::string endpoint, const Json::Value& parameters)
{
	// Set URL
	curl_easy_setopt(m_curl, CURLOPT_URL, (m_url + endpoint).c_str());

	// Set POST Content
	m_headers = curl_slist_append(m_headers, "Content-Type: text/json");

	Json::FastWriter fastWriter;
	m_paramString = fastWriter.write(parameters);
	curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_paramString.c_str());
}

CURLcode DbConnection::curlPerformRequest()
{
	m_recvbufJson.str(""); // clear any old results
	if(m_headers != NULL) curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
	return curl_easy_perform(m_curl);
}

long DbConnection::curlGetHttpStatusCode()
{
	long httpresult;
	curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpresult);
	return httpresult;
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
	m_url = url; // base URL

	// set POST parameters
	Json::Value parameters;
	parameters["userName"] = user;
	parameters["password"] = password;

	// set up curl
	curlReset();
	curlPrepareReceiveJson();
	curlPreparePost("auth/login", parameters);
	CURLcode result = curlPerformRequest();

	long httpresult = curlGetHttpStatusCode();
	if (result != CURLE_OK || httpresult < 200 || httpresult > 299) return false;

	Json::Reader reader;
	Json::Value response;
	if (!reader.parse(m_recvbufJson.str(), response, false)) // parse responseString into response, skipping comments
	{
		return false;
	}

	m_authToken = response["Token"].asString();
	return true;
}

bool DbConnection::executeCommand(Json::Value& apiResult, const std::string & command, const Json::Value & parameters, bool isPost)
{
	curlReset();
	curlPrepareReceiveJson();
	curlPrepareAuth();
	if(isPost)
		curlPreparePost(std::string("odbc/") + command, parameters);
	else
		curlPrepareGet(std::string("odbc/") + command, parameters);
	CURLcode curlresult = curlPerformRequest();
	long httpresult = curlGetHttpStatusCode();
	if (curlresult != CURLE_OK || httpresult < 200 || httpresult > 299)
	{
		apiResult = Json::Value(); // clear result object if parsing failed
		return false;
	}

	Json::Reader reader;
	if (!reader.parse(m_recvbufJson.str(), apiResult, false)) // parse responseString into response, skipping comments
	{
		apiResult = Json::Value(); // clear result object if parsing failed
		return false;
	}

	return true;
}

bool DbConnection::executeGetCommand(Json::Value& apiResult, const std::string & command, const Json::Value & parameters)
{
	return executeCommand(apiResult, command, parameters, false);
}

bool DbConnection::executePostCommand(Json::Value& apiResult, const std::string & command, const Json::Value & parameters)
{
	return executeCommand(apiResult, command, parameters, true);
}

