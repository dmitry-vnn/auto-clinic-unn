#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#include "patient.h"
#include "request_type.h"

#pragma comment(lib, "ws2_32.lib")

class BadResponse final : std::logic_error
{

private:
	ServerResponseType _responseType;


public:
	BadResponse(ServerResponseType responseType) :
		logic_error(""), _responseType(responseType) {}

	BadResponse(const std::string& message, ServerResponseType responseType) :
		logic_error(message),	_responseType(responseType)	{}

	BadResponse(const char* message, ServerResponseType responseType):
		logic_error(message),	_responseType(responseType)	{}

	ServerResponseType GetResponseType() const
	{
		return _responseType;
	}
};

class ClientService {

private:

	SOCKET _serverSocket;

	std::string _host;
	std::string _port;

	size_t _connectionAttempts;
	size_t _timeDelayMillisAttempt;

	char _buffer[72 * 1024];

public:

	ClientService(std::string host, const size_t port, const size_t connectionAttempts, const size_t timeDelayMillisAttempt) :
		_serverSocket(INVALID_SOCKET),	_host(std::move(host)), _port(std::to_string(port)),
		_connectionAttempts(connectionAttempts), _timeDelayMillisAttempt(timeDelayMillisAttempt),
		_buffer()
		{ }

	ClientService(const size_t port, const size_t connectionAttempts, const size_t timeDelayMillisAttempt) :
		ClientService("127.0.0.1", port, connectionAttempts, timeDelayMillisAttempt) {}

	ClientService(const ClientService& other) = delete;
	ClientService& operator=(const ClientService& other) = delete;

public:
	bool TryStart();

	std::string AuthorizeAndGetPersonalData(std::string preparedAuthData);
	std::string GetPatientAsString();
	void Logout();
};
