#pragma once

#include "json/json.h"
#include "Environment.h"

#include <stdint.h>
#include <set>
#include <string>
#include <mutex>

#include <curl/curl.h>

static size_t ReceiveJson(void *contents, size_t size, size_t nmemb, void *userp);
static size_t ReceiveData(void *contents, size_t size, size_t nmemb, void *userp);
static size_t SendData(void *contents, size_t size, size_t nmemb, void *userp);
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

	

	// CURL connection and buffers needed for HTTP I/O
	CURL *m_curl;
	// special curl handles for uploads and downloads
	CURL *m_curlTransfer;
	CURLM *m_curlTransferMulti; 

	curl_slist *m_headers;
	std::stringstream m_recvbufJson; // received json data will be stored here

	// Binary data is downloaded into m_transferBuf which points to an application-
	// supplied buffer. Excess data is written into m_transferBufOverflow and copied
	// to the application's buffer in the next downloadChunk() call.
	uint8_t* m_transferBuf;
	uint64_t m_transferBufLength; // remaining size of m_transferBuf in bytes
	std::vector<uint8_t> m_transferBufOverflow;
	// offset from which to start reading data into the application buffer
	size_t m_transferBufOverflowOffset; 

	// Indicates whether the curl handle is currently downloading/uploading a binary.
	bool m_isDownloadPending, m_isUploadPending;

	// Indicates that the current upload is complete. The sendData function uses this
	// to decide whether to end the transfer.
	bool m_isUploadFinished; 


	bool executeCommand(Json::Value& result, const std::string & command, const Json::Value & parameters, bool isPost);
	size_t receiveJson(void *contents, size_t size);
	size_t receiveData(void *contents, size_t size);
	size_t sendData(void *contents, size_t size);

	void curlReset(CURL* curl);
	void curlPrepareReceiveJson(CURL* curl);
	void curlPrepareReceiveData(CURL* curl);
	void curlPrepareSendData(CURL* curl);
	void curlPrepareAuth(CURL* curl);
	void curlPrepareGet(CURL* curl, std::string endpoint, const Json::Value& parameters);
	void curlPreparePost(CURL* curl, std::string endpoint, const Json::Value& parameters);
	void curlPrepareBinaryPost(CURL* curl, std::string endpoint);
	CURLcode curlPerformRequest(CURL* curl);
	long curlGetHttpStatusCode(CURL* curl);

	// helper methods
	std::string encodeGetParameters(const Json::Value& parameters);

	/* CURL needs a plain callback function for handling received data => add that function as a friend 
	 * so that it can access DbConnection::receiveJson(). */
	friend size_t ReceiveJson(void *contents, size_t size, size_t nmemb, void *userp);
	friend size_t ReceiveData(void *contents, size_t size, size_t nmemb, void *userp);
	friend size_t SendData(void *contents, size_t size, size_t nmemb, void *userp);

public:
	DbConnection(Environment* env);
	~DbConnection();

	Statement* createStatement();
	void removeStatement(Statement* stmt);

	void setTimeout(uint32_t timeout);

// commands
	bool connect(std::string url, std::string user, std::string password);
	bool executeGetCommand(Json::Value& result, const std::string & command, const Json::Value & parameters);
	bool executePostCommand(Json::Value& result, const std::string & command, const Json::Value & parameters);

// binary up/download
	void resetTransfer();
	bool beginDownload(const std::string & path, int64_t offset = 0, int64_t size = 0);
	int64_t downloadChunk(void* result, uint64_t size, bool* completed = NULL);

	bool beginUpload(const std::string & handle);
	bool finishUpload();
	bool uploadChunk(void* data, int64_t size);

};

