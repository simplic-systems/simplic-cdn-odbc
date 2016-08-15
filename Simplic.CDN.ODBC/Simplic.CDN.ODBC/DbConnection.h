#pragma once

#include "json/json.h"
#include "Environment.h"

#include <stdint.h>
#include <set>
#include <string>
#include <mutex>



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
	uint32_t m_timeout;

	// current result set
	Json::Value m_apiResult;

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

