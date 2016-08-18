#pragma once

#include "json/json.h"
#include "Environment.h"

#include <stdint.h>
#include <set>
#include <string>
#include <mutex>

#include <curl/curl.h>

static size_t ReceiveJson(void *contents, size_t size, size_t nmemb, void *userp);
class Statement;

class DbConnection
{
private:
	std::recursive_mutex m_mutex;

	Environment* m_environment;
	std::set<Statement*> m_statements;


	// connection properties
	std::string m_url;
	std::string m_authToken;
	std::string m_paramString;
	uint32_t m_timeout;

	// current result set
	Json::Value m_apiResult;

	// CURL connection and buffers needed for HTTP I/O
	CURL *m_curl;
	curl_slist *m_headers;
	std::stringstream m_recvbufJson; // received json data will be stored here

	size_t receiveJson(void *contents, size_t size);

	void curlReset();
	void curlPrepareReceiveJson();
	void curlPrepareAuth();
	void curlPrepareGet(std::string endpoint, const Json::Value& parameters);
	void curlPreparePost(std::string endpoint, const Json::Value& parameters);
	CURLcode curlPerformRequest();

	long curlGetHttpStatusCode();

	// helper methods
	std::string encodeGetParameters(const Json::Value& parameters);

	/* CURL needs a plain callback function for handling received data => add that function as a friend 
	 * so that it can access DbConnection::receiveJson(). */
	friend size_t ReceiveJson(void *contents, size_t size, size_t nmemb, void *userp);

public:
	DbConnection(Environment* env);
	~DbConnection();

	Statement* createStatement();
	void removeStatement(Statement* stmt);

	void setTimeout(uint32_t timeout);

	
	inline Json::Value* getApiResult() { return &m_apiResult; }
// commands
	bool connect(std::string url, std::string user, std::string password);
	Json::Value * executeCommand(const std::string & command, const Json::Value & parameters);
	bool fetch(std::vector<Json::Value*>& result, uint32_t fromRow, uint32_t numRows);


};

