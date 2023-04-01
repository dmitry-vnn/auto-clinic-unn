#include "client_service.h"

#include "doctor.h"
#include "request_type.h"

namespace client_internal
{
	bool TryStartSocketLib()
	{
		WSADATA wsaData; //Служебная структура

		int resultCode = WSAStartup(MAKEWORD(2, 2), &wsaData); //Функция запуска библиотеки сокета, возвращает результат отличный от нуля, если не удалось 

		if (resultCode != 0)
		{
			std::cerr << "Ошибка запуска сервера: Бибилотека ws2_32.lib не может быть использована" << std::endl;
			return false;
		}

		return true;
	}




	//Попытка проинициализировать дескриптор серверного сокета на клиенте
	SOCKET TryInitServerSocket(const char* host, const char* port, size_t attempts, size_t attemptsDelayMillis)
	{
		addrinfo* resultAddressInfo = nullptr;

		//Структура, описывающая реальный адрес сокета (протокол, семейство адресов (ipv4))
		addrinfo addressInfo{};


		addressInfo.ai_family = AF_INET;
		addressInfo.ai_socktype = SOCK_STREAM;
		addressInfo.ai_protocol = IPPROTO_TCP;

		/*
		 * getaddrinfo cохраняет сведения об реальном адресе сокета
		 * Указателю resultAddressInfo меняет адрес на реальный указатель на этот адрес
		 */

		int resultCode = getaddrinfo(host, port, &addressInfo, &resultAddressInfo);

		if (resultCode != 0) {

			std::cerr << "Ошибка запуска клиента: Физический адрес сервера не может быть использован" << std::endl;
			std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

			WSACleanup();

			return INVALID_SOCKET;
		}

		/*
		* Попытка создать дескриптор серверного сокета по ранее инициализированному адресу
		*/
		SOCKET serverSocket = socket(resultAddressInfo->ai_family, resultAddressInfo->ai_socktype, resultAddressInfo->ai_protocol);

		if (serverSocket == INVALID_SOCKET)
		{

			std::cerr << "Ошибка запуска клиента: Дескриптор серверного сокета не может быть создан" << std::endl;
			std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

			freeaddrinfo(resultAddressInfo);
			WSACleanup();

			return INVALID_SOCKET;
		}


		/*
		 * Привязка дескриптора серверного сокета к реальному серверному сокету (расположенному на сервере)
		 * Т.е подключение сокета к реальному серверу
		 */

		while (attempts-- != 0)
		{
			resultCode = connect(serverSocket, resultAddressInfo->ai_addr, static_cast<int>(resultAddressInfo->ai_addrlen));

			if (resultCode == SOCKET_ERROR)
			{
				std::cerr << "Ошибка запуска клиента: Не удалось подключиться к серверу" << std::endl;
				std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

				if (attempts != 0)
				{
					std::cerr << "Попыток осталось: " << attempts << std::endl;
					Sleep(attemptsDelayMillis);
				}
			} else
			{
				break;
			}
		}

		freeaddrinfo(resultAddressInfo);

		if (resultCode == SOCKET_ERROR)
		{
			closesocket(serverSocket);
			WSACleanup();
			return INVALID_SOCKET;
		}

		return serverSocket;
	}

}


bool ClientService::TryStart()
{

	if (!client_internal::TryStartSocketLib())
	{
		return false;
	}

	_serverSocket = client_internal::TryInitServerSocket(_host.c_str(), _port.c_str(), _connectionAttempts, _timeDelayMillisAttempt);

	return _serverSocket != INVALID_SOCKET;
}

#define err(x) std::cout << "\x1B[91m" << (x) << "\033[0m"
#define errln(x) err(x) << std::endl

std::string ClientService::AuthorizeAndGetPersonalData(std::string preparedAuthData)
{
	char clientRequestTypeAsByte = static_cast<char>(ClientRequestType::AUTH);

	_buffer[0] = static_cast<char>(ClientRequestType::AUTH);
	std::copy(preparedAuthData.begin(), preparedAuthData.end(), _buffer + 1);
	_buffer[preparedAuthData.size() + 1] = '\0';

	int errorCode = send(_serverSocket, _buffer, 1 + preparedAuthData.size() + 1, 0);

	if (errorCode == SOCKET_ERROR)
	{
		err("	Ошибка: Не удалось отправить запрос на авторизацию") << std::endl;
		err("	Сообщение ошибки: ") << gai_strerrorA(WSAGetLastError()) << std::endl;

		throw BadResponse(ServerResponseType::NO_RESPONSE_ERR);
	}

	char serverResponseTypeAsByte;
	int bytesReceived = recv(_serverSocket, &serverResponseTypeAsByte, 1, 0);

	if (bytesReceived <= 0)
	{

		if (bytesReceived < 0)
		{
			err("	Ошибка: Не удалось авторизовать доктора") << std::endl;
			err("	Сообщение ошибки: ") << gai_strerrorA(WSAGetLastError()) << std::endl;
		}

		throw BadResponse(ServerResponseType::NO_RESPONSE_ERR);

	}

	auto serverResponseType = static_cast<ServerResponseType>(serverResponseTypeAsByte);

	if (serverResponseType != ServerResponseType::AUTH_OK)
	{
		throw BadResponse(serverResponseType);
	}

	bytesReceived = recv(_serverSocket, _buffer, sizeof(_buffer), 0);


	if (bytesReceived <= 0)
	{

		if (bytesReceived < 0)
		{
			err("	Ошибка: Не удалось получить персональные данные") << std::endl;
			err("	Сообщение ошибки: ") << gai_strerrorA(WSAGetLastError()) << std::endl;
		}

		throw BadResponse(ServerResponseType::NO_RESPONSE_ERR);

	}

	return { _buffer, _buffer + bytesReceived };

}

std::string ClientService::GetPatientAsString()
{

	char clientRequestTypeAsByte = static_cast<char>(ClientRequestType::GET_PATIENT);

	int errorCode = send(_serverSocket, &clientRequestTypeAsByte, 1, 0);

	if (errorCode == SOCKET_ERROR)
	{
		err("	Ошибка: Не удалось отправить запрос на получение пациента") << std::endl;
		err("	Сообщение ошибки: ") << gai_strerrorA(WSAGetLastError()) << std::endl;

		throw BadResponse(ServerResponseType::NO_RESPONSE_ERR);
	}

	char serverResponseTypeAsByte;
	int bytesReceived = recv(_serverSocket, &serverResponseTypeAsByte, 1, 0);


	if (bytesReceived <= 0)
	{

		if (bytesReceived < 0)
		{
			err("	Ошибка: Не удалось получить пациента") << std::endl;
			err("	Сообщение ошибки: ") << gai_strerrorA(WSAGetLastError()) << std::endl;
		}

		throw BadResponse(ServerResponseType::NO_RESPONSE_ERR);

	}

	auto serverResponseType = static_cast<ServerResponseType>(serverResponseTypeAsByte);

	if (serverResponseType == ServerResponseType::GET_PATIENT_OK)
	{
		bytesReceived = recv(_serverSocket, _buffer, sizeof(_buffer), 0);

		if (bytesReceived <= 0)
		{

			if (bytesReceived < 0)
			{
				err("	Ошибка: Не удалось получить пациента") << std::endl;
				err("	Сообщение ошибки: ") << gai_strerrorA(WSAGetLastError()) << std::endl;
			}

			throw BadResponse(ServerResponseType::NO_RESPONSE_ERR);
		}

		return { _buffer, _buffer + bytesReceived };
	}

	throw BadResponse(serverResponseType);
}

void ClientService::Logout()
{

	char clientRequestTypeAsByte = static_cast<char>(ClientRequestType::LOGOUT);

	send(_serverSocket, &clientRequestTypeAsByte, 1, 0);
}


/*

	std::string s = "Hello, from ClientService";

	Sleep(10000);

	int bytesSend = send(connectSocket, s.c_str(), static_cast<int>(s.length()), 0);

	if (bytesSend == SOCKET_ERROR) {

		closesocket(connectSocket);
		WSACleanup();

		throw std::logic_error("send failed: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "bytes send: " << bytesSend << std::endl;

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	resultCode = shutdown(connectSocket, SD_SEND);
	if (resultCode == SOCKET_ERROR) {

		closesocket(connectSocket);
		WSACleanup();

		throw std::logic_error("shutdown failed: " + std::to_string(WSAGetLastError()));
	}

	// // Receive data until the server closes the connection
	// do {
	// 	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	// 	if (iResult > 0)
	// 		printf("Bytes received: %d\n", iResult);
	// 	else if (iResult == 0)
	// 		printf("Connection closed\n");
	// 	else
	// 		printf("recv failed: %d\n", WSAGetLastError());
	// } while (iResult > 0);
	*/
