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

/// Function called by CURL when it wants to send binary data.
static size_t SendData(void *contents, size_t size, size_t nmemb, void *userp)
{
	return ((DbConnection*)userp)->sendData(contents, size*nmemb);
}

DbConnection::DbConnection(Environment * env)
	: m_timeout(10)
{
	m_environment = env;
	m_curl = curl_easy_init();
	m_curlTransfer = curl_easy_init();
	m_curlTransferMulti = curl_multi_init();
	
	m_headers = NULL;
	m_transferBuf = NULL;
	m_transferBufLength = 0;
	m_isDownloadPending = false;
	m_isUploadPending = false;
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
	if(m_transferBuf == NULL || m_transferBufLength == 0)
		return CURL_WRITEFUNC_PAUSE;
		
	
	// copy data to application buffer
	size_t recvbufBytes = min(size, m_transferBufLength);
	memcpy(m_transferBuf, pContents, recvbufBytes);
	
	// advance pointers
	pContents += recvbufBytes;
	m_transferBuf += recvbufBytes;

	// update length of remaining part of application buffer
	m_transferBufLength -= recvbufBytes;

	// copy remaining data to overflow buffer
	size_t overflowBytes = size - recvbufBytes;
	m_transferBufOverflow.resize(overflowBytes);

	if (overflowBytes > 0)
	{
		memcpy(m_transferBufOverflow.data(), pContents, overflowBytes);
	}

	return size; // if we return less than size, curl will assume that there's an error.
}

size_t DbConnection::sendData(void *contents, size_t size)
{
	if (!m_isUploadPending) return CURL_READFUNC_ABORT; // something must be going wrong if this happens.
	if (m_isUploadFinished)
	{
		m_isUploadPending = false;
		return 0; // signals to curl that the upload is finished
	}

	/* if there is no data to send, return CURL_READFUNC_PAUSE. This will cause curl to
	 * yield control back to us, allowing us to keep going and wait for more data
	   from the application */
	if (m_transferBuf == NULL || m_transferBufLength == 0)
		return CURL_READFUNC_PAUSE;

	// copy data from application buffer
	size_t transferBytes = min(size, m_transferBufLength);
	memcpy(contents, m_transferBuf, transferBytes);

	// advance pointer to application buffer for next read
	m_transferBuf += transferBytes;
	m_transferBufLength -= transferBytes;

	// return number of bytes to send
	return transferBytes;
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

void DbConnection::curlPrepareSendData(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, SendData);
	curl_easy_setopt(curl, CURLOPT_READDATA, (void*)this);
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

void DbConnection::curlPrepareBinaryPost(CURL* curl, std::string endpoint)
{
	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// Set URL
	curl_easy_setopt(curl, CURLOPT_URL, (m_url + endpoint).c_str());

	// Set POST Content Type
	m_headers = curl_slist_append(m_headers, "Content-Type: application/octet-stream");
	m_headers = curl_slist_append(m_headers, "Transfer-Encoding: chunked");
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

	if (m_headers != NULL) curl_slist_free_all(m_headers);
	m_headers = NULL;

	m_curlTransfer = curl_easy_init();
	m_curlTransferMulti = curl_multi_init();
	curl_multi_add_handle(m_curlTransferMulti, m_curlTransfer);
	
	
	m_transferBuf = NULL;
	m_transferBufLength = 0;
	m_transferBufOverflowOffset = 0;
	m_transferBufOverflow.clear();
	m_isDownloadPending = false;
	m_isUploadPending = false;
	m_isUploadFinished = false;

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
	m_transferBufLength = size;
	m_transferBuf = (uint8_t*) result;
	if(completed) *completed = false;

	// copy overflow data that didn't fit into the buffer the last time this function was called
	m_transferBufOverflowOffset = min(m_transferBufOverflowOffset, m_transferBufOverflow.size());
	int64_t overflowBytes = min(size, m_transferBufOverflow.size() - m_transferBufOverflowOffset);

	memcpy(m_transferBuf, m_transferBufOverflow.data() + m_transferBufOverflowOffset, overflowBytes);
	
	m_transferBuf += overflowBytes;
	m_transferBufOverflowOffset += overflowBytes;
	m_transferBufLength -= overflowBytes;

	// clear overflow buffer if we have read it completely
	if (m_transferBufOverflowOffset >= m_transferBufOverflow.size())
	{
		m_transferBufOverflowOffset = 0;
		m_transferBufOverflow.clear();
	}
	

	// don't try to download more data if the download has already completed.
	if (!m_isDownloadPending)
	{
		if (completed && m_transferBufOverflow.size() == 0) *completed = true;
		return size - m_transferBufLength;
	}




	int nRunningHandles = 1;
	/* TODO: Add some sort of timeout in order to prevent extremely slow / stuck transfers from
	   locking up the application */
	// Run the CURL transfer until it finishes or our buffer fills up
	while (nRunningHandles > 0 && m_transferBufLength > 0)
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
			if (completed && m_transferBufOverflow.size() == 0) *completed = true;
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
		if (m_transferBufLength > 0)
		{
			resetTransfer();
			return -1;
		}
	}

	return size - m_transferBufLength;
}

bool DbConnection::beginUpload(const std::string & handle)
{
	resetTransfer();
	curlReset(m_curlTransfer);
	
	m_headers = curl_slist_append(m_headers, (std::string("x-parameter-handle: ") + handle).c_str());

	curlPrepareAuth(m_curlTransfer);
	curlPrepareBinaryPost(m_curlTransfer, "cdn/set");
	curlPrepareSendData(m_curlTransfer);

	if (m_headers != NULL) curl_easy_setopt(m_curlTransfer, CURLOPT_HTTPHEADER, m_headers);


	int nRunningHandles = 0;
	CURLMcode curlresult = curl_multi_perform(m_curlTransferMulti, &nRunningHandles);

	if (curlresult != CURLM_OK)
	{
		resetTransfer();
		return false;
	}

	m_isUploadPending = true;
	m_isUploadFinished = false;
	return true;
}

bool DbConnection::finishUpload()
{
	if (!m_isUploadPending) return false;
	
	// this will make the sendData function finish the upload
	m_isUploadFinished = true;

	int nRunningHandles = 0;
	int nAttempts = 0;
	
	// curl_multi_perform will call sendData, which will set m_isUploadPending to false on success.
	// CURL might be busy right now so if sendData doesn't get invoked, we will retry once.
	while (m_isUploadPending && nAttempts < 2)
	{
		CURLMcode curlresult = curl_multi_perform(m_curlTransferMulti, &nRunningHandles);

		if (curlresult != CURLM_OK)
		{
			resetTransfer();
			return false;
		}

		if (m_isUploadPending) // nothing happened => curl seems to be busy, wait for curl
		{
			int numfds = 0;
			curl_multi_wait(m_curlTransfer, NULL, 0, m_timeout * 1000, &numfds);
		}
	}

	// abort if this didn't work.
	if (m_isUploadPending)
	{
		resetTransfer();
		return false;
	}

	return true;
}

bool DbConnection::uploadChunk(void * data, int64_t size)
{
	if (!m_isUploadPending) return false;
	m_transferBuf = (uint8_t*) data;
	m_transferBufLength = size;

	curl_easy_pause(m_curlTransfer, CURLPAUSE_CONT); // continue transfer if it's paused.
	int nRunningHandles = 1;

	while (m_transferBufLength > 0 && nRunningHandles > 0)
	{
		CURLMcode curlresult = curl_multi_perform(m_curlTransferMulti, &nRunningHandles);

		// failure or stall => abort
		if (curlresult != CURLM_OK)
		{
			resetTransfer();
			return false;
		}

		// wait for curl if it has not uploaded all availabe data yet.
		if (m_transferBufLength > 0)
		{
			int numfds = 0; 
			curl_multi_wait(m_curlTransfer, NULL, 0, m_timeout * 1000, &numfds);
		}
	}

	// fail if we somehow didn't manage to transfer all data
	if (m_transferBufLength > 0)
	{
		resetTransfer();
		return false;
	}

	return true;
}
