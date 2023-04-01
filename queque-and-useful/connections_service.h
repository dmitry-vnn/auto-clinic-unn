#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#include "queue.h"
#include "repository.h"

#pragma comment(lib, "ws2_32.lib")

class ConnectionsService
{
private:

	Repository* _repository;
	PatientConverter _converter;

	std::string _port;


	SOCKET _serverSocket;

private:

	const size_t _bufferSize = 72 * 1024;

	char _buffer[72 * 1024];	//allocate 72 Kilobyte buffer

	std::vector<SOCKET> _clients,
						_authorizedClients;




public:
	ConnectionsService(size_t port, Repository* repository) :

		_repository(repository), _port(std::to_string(port)),
		_serverSocket(INVALID_SOCKET), _buffer()
	{
		assert(repository != nullptr, "repository must be not null");
	}

	bool TryStart();
	void StartListenConnections();

	ConnectionsService(const ConnectionsService& other) = delete;
	ConnectionsService& operator=(const ConnectionsService& other) = delete;

private:

};
