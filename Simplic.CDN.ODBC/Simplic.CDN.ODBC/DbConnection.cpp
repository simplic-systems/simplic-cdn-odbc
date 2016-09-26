#include "stdafx.h"
#include "DbConnection.h"
#include "Statement.h"

#include "json/json.h"

/// Function called by CURL when JSON data has been received.
static size_t ReceiveJson(void *contents, size_t size, size_t nmemb, void *userp)
{
	return ((DbConnection*)userp)->receiveJson(contents, size*nmemb);
}

/// Function called by CURL when binary data has been received.
static size_t ReceiveData(void *contents, size_t size, size_t nmemb, void *userp)
{
	return ((DbConnection*)userp)->receiveData(contents, size*nmemb);
}

DbConnection::DbConnection(Environment * env)
	: m_timeout(10)
{
	m_environment = env;
	m_curl = curl_easy_init();
	m_curlTransfer = curl_easy_init();
	m_curlTransferMulti = curl_multi_init();
	
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

size_t DbConnection::receiveData(void *contents, size_t size)
{
	uint8_t* pContents = (uint8_t*) contents;

	/* if there is no application buffer to put the data into, return CURL_WRITEFUNC_PAUSE
	   in order to signal that we couldn't process this chunk of data.
	   It will be delivered to the receiveData function again when the transfer is unpaused. */
	if(m_recvbuf == NULL || m_recvbufLength == 0)
		return CURL_WRITEFUNC_PAUSE;
		
	
	// copy data to application buffer
	size_t recvbufBytes = min(size, m_recvbufLength);
	memcpy(m_recvbuf, pContents, recvbufBytes);
	
	// advance pointers
	pContents += recvbufBytes;
	m_recvbuf += recvbufBytes;

	// update length of remaining part of application buffer
	m_recvbufLength -= recvbufBytes;

	// copy remaining data to overflow buffer
	size_t overflowBytes = size - recvbufBytes;
	m_recvbufOverflow.resize(overflowBytes);

	if (overflowBytes > 0)
	{
		memcpy(m_recvbufOverflow.data(), pContents, overflowBytes);
	}

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
		const Json::Value& jsonValue = *it;
		if (jsonValue.type() == Json::stringValue)
		{
			escaped = curl_easy_escape(m_curl, jsonValue.asCString(), 0);
		}
		else
		{
			Json::FastWriter fastWriter;
			fastWriter.omitEndingLineFeed();
			std::string value = fastWriter.write(jsonValue);
			escaped = curl_easy_escape(m_curl, value.c_str(), 0);
		}

		ss << "=" << escaped;
		curl_free(escaped);

		separator = '&'; // use '&' to separate key-value pairs
	}
	return ss.str();
}

void DbConnection::curlReset(CURL* curl)
{
	curl_easy_reset(curl);
	if (m_headers != NULL) curl_slist_free_all(m_headers);
	m_headers = NULL;
}

void DbConnection::curlPrepareReceiveJson(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ReceiveJson);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)this);
}

void DbConnection::curlPrepareReceiveData(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ReceiveData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)this);
}

void DbConnection::curlPrepareAuth(CURL* curl)
{
	m_headers = curl_slist_append(m_headers, (std::string("Authorization: jwt ") + m_authToken).c_str());
}

void DbConnection::curlPrepareGet(CURL* curl, std::string endpoint, const Json::Value& parameters)
{
	std::string parameterString = encodeGetParameters(parameters);
	curl_easy_setopt(curl, CURLOPT_URL, (m_url + endpoint + parameterString).c_str());
}

void DbConnection::curlPreparePost(CURL* curl, std::string endpoint, const Json::Value& parameters)
{
	// Set URL
	curl_easy_setopt(curl, CURLOPT_URL, (m_url + endpoint).c_str());

	// Set POST Content
	m_headers = curl_slist_append(m_headers, "Content-Type: text/json");

	Json::FastWriter fastWriter;
	m_paramString = fastWriter.write(parameters);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_paramString.c_str());
}

CURLcode DbConnection::curlPerformRequest(CURL* curl)
{
	m_recvbufJson.str(""); // clear any old results
	if(m_headers != NULL) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, m_headers);
	return curl_easy_perform(curl);
}


long DbConnection::curlGetHttpStatusCode(CURL* curl)
{
	long httpresult;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpresult);
	return httpresult;
}


void DbConnection::resetTransfer()
{
	//curlReset(m_curlTransfer);

	curl_multi_remove_handle(m_curlTransferMulti, m_curlTransfer);
	curl_easy_cleanup(m_curlTransfer);
	curl_multi_cleanup(m_curlTransferMulti);

	m_curlTransfer = curl_easy_init();
	m_curlTransferMulti = curl_multi_init();
	curl_multi_add_handle(m_curlTransferMulti, m_curlTransfer);
	
	
	m_recvbuf = NULL;
	m_recvbufLength = 0;
	m_recvbufOverflowOffset = 0;
	m_recvbufOverflow.clear();
	m_isDownloadPending = false;


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
	curlReset(m_curl);
	curlPrepareReceiveJson(m_curl);
	curlPreparePost(m_curl, "auth/login", parameters);
	CURLcode result = curlPerformRequest(m_curl);

	long httpresult = curlGetHttpStatusCode(m_curl);
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
	curlReset(m_curl);
	curlPrepareReceiveJson(m_curl);
	curlPrepareAuth(m_curl);
	if(isPost)
		curlPreparePost(m_curl, std::string("odbc/") + command, parameters);
	else
		curlPrepareGet(m_curl, std::string("odbc/") + command, parameters);
	CURLcode curlresult = curlPerformRequest(m_curl);
	long httpresult = curlGetHttpStatusCode(m_curl);
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




bool DbConnection::beginDownload(const std::string & path, int64_t offset, int64_t size)
{
	Json::Value parameters(Json::objectValue);
	parameters["path"] = path;
	parameters["offset"] = offset;
	parameters["size"] = size;

	resetTransfer();

	
	curlPrepareReceiveData(m_curlTransfer);
	curlPrepareAuth(m_curlTransfer);
	curlPrepareGet(m_curlTransfer, "cdn/get", parameters);

	if (m_headers != NULL) curl_easy_setopt(m_curlTransfer, CURLOPT_HTTPHEADER, m_headers);

	int nRunningHandles = 0;
	CURLMcode curlresult = curl_multi_perform(m_curlTransferMulti, &nRunningHandles);

	if (curlresult != CURLM_OK)
	{
		resetTransfer();
		return false;
	}

	m_isDownloadPending = true;
	return true;
}


int64_t DbConnection::downloadChunk(void* result, uint64_t size, bool* completed)
{
	m_recvbufLength = size;
	m_recvbuf = (uint8_t*) result;
	if(completed) *completed = false;

	// copy overflow data that didn't fit into the buffer the last time this function was called
	m_recvbufOverflowOffset = min(m_recvbufOverflowOffset, m_recvbufOverflow.size());
	int64_t overflowBytes = min(size, m_recvbufOverflow.size() - m_recvbufOverflowOffset);

	memcpy(m_recvbuf, m_recvbufOverflow.data() + m_recvbufOverflowOffset, overflowBytes);
	
	m_recvbuf += overflowBytes;
	m_recvbufOverflowOffset += overflowBytes;
	m_recvbufLength -= overflowBytes;

	// clear overflow buffer if we have read it completely
	if (m_recvbufOverflowOffset >= m_recvbufOverflow.size())
	{
		m_recvbufOverflowOffset = 0;
		m_recvbufOverflow.clear();
	}
	

	// don't try to download more data if the download has already completed.
	if (!m_isDownloadPending)
	{
		if (completed && m_recvbufOverflow.size() == 0) *completed = true;
		return size - m_recvbufLength;
	}




	int nRunningHandles = 1;
	/* TODO: Add some sort of timeout in order to prevent extremely slow / stuck transfers from
	   locking up the application */
	// Run the CURL transfer until it finishes or our buffer fills up
	while (nRunningHandles > 0 && m_recvbufLength > 0)
	{
		curl_easy_pause(m_curlTransfer, CURLPAUSE_CONT); // continue transfer if it's paused.
		CURLMcode curlresult = curl_multi_perform(m_curlTransferMulti, &nRunningHandles);

		if (curlresult != CURLM_OK)
		{
			resetTransfer();
			return -1;
		}
	}

	// read and process all available messages in order to find out whether the transfer has completed.
	int msgsInQueue = 0;
	bool transferDone = false;
	while (CURLMsg* msg = curl_multi_info_read(m_curlTransferMulti, &msgsInQueue))
	{
		if (msg->msg == CURLMSG_DONE)
		{
			transferDone = true;
			m_isDownloadPending = false;
			// if the overflow buffer is empty too, the transfer is completed.
			if (completed && m_recvbufOverflow.size() == 0) *completed = true;
		}
	}


	// check the HTTP status if the transfer is completed.
	if (transferDone)
	{
		long httpresult = curlGetHttpStatusCode(m_curlTransfer);

		if (httpresult < 200 || httpresult > 299) // Bad HTTP result => fail
		{
			resetTransfer();
			return -1;
		}
	}
	else
	{
		// if the curl handle is still downloading but didn't manage
		// to fill our buffer, something must have gone wrong.
		if (m_recvbufLength > 0)
		{
			resetTransfer();
			return -1;
		}
	}

	return size - m_recvbufLength;
}