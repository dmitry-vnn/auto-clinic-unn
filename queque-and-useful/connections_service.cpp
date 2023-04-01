#include "connections_service.h"

#include "request_type.h"

namespace connections_internal
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




	//Возвращает SOCKET_ERROR если не удалось
	SOCKET TryBindServerSocketAndListenIt(const char* port)
	{
		addrinfo* resultAddressInfo = nullptr;

		//Структура, описывающая реальный адрес сокета (протокол, семейство адресов (ipv4))
		addrinfo addressInfo{};

		addressInfo.ai_family = AF_INET;
		addressInfo.ai_socktype = SOCK_STREAM;
		addressInfo.ai_protocol = IPPROTO_TCP;
		addressInfo.ai_flags = AI_PASSIVE;

		/*
		 * getaddrinfo cохраняет сведения об реальном адресе сокета
		 * Указателю resultAddressInfo меняет адрес на реальный указатель на этот адрес
		 */

		int resultCode = getaddrinfo(nullptr, port, &addressInfo, &resultAddressInfo);

		if (resultCode != 0) {

			std::cerr << "Ошибка запуска сервера: Физический адрес сервера не может быть использован" << std::endl;
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

			std::cerr << "Ошибка запуска сервера: Сокет не может быть создан" << std::endl;
			std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

			freeaddrinfo(resultAddressInfo);
			WSACleanup();

			return INVALID_SOCKET;
		}

		/*
		 * Привязка сокета к реальному адресу и порту. Биндинг
		 */
		resultCode = bind(serverSocket, resultAddressInfo->ai_addr, static_cast<int>(resultAddressInfo->ai_addrlen));

		if (resultCode == SOCKET_ERROR)
		{

			std::cerr << "Ошибка запуска сервера: Сокет не может быть привязан к данному адресу и порту" << std::endl;
			std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

			freeaddrinfo(resultAddressInfo);
			closesocket(serverSocket);
			WSACleanup();

			return INVALID_SOCKET;

		}


		//Освобождаем указатель, ссылающийся на реальную структуру с адресом сокета. Сокет проинициализирован, эта структура больше не нужна
		freeaddrinfo(resultAddressInfo);


		/*-------------------------
		* Устанавливает режим ввода/вывода для сокета
		* FIONBIO - позволяет включить/выключить блокирующий
		* режим, для численного значения параментра socketMode
		*
		* Если socketMode = 0  => блокирующий режим;
		* Если socketMode != 0  => неблокирующий режим.
		*/


		unsigned long socketMode = 1; //Не блокирующий режим методов сокета (accept, recv, send)

		resultCode = ioctlsocket(serverSocket, FIONBIO, &socketMode);

		if (resultCode == SOCKET_ERROR)
		{
			std::cerr << "Ошибка запуска сервера: Режим сокета не может быть сменён на неблокирующий" << std::endl;
			std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

			closesocket(serverSocket);
			WSACleanup();

			return INVALID_SOCKET;
		}


		/*
		* Функция listen переводит сокет в состояние прослушки входящих сообщений
		* SOMAXCONN - константа, указывающая функции listen, что число соединений с сервером - максимально (т.е (2^31 - 1) соединений)
		*/
		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) //Пытаемся слушать сокет
		{

			std::cerr << "Ошибка запуска сервера: Сокет не может быть привязан к данному адресу и порту" << std::endl;
			std::cerr << "Сообщение ошибки: " << gai_strerrorA(WSAGetLastError()) << std::endl;

			closesocket(serverSocket);
			WSACleanup();

			return INVALID_SOCKET;
		}

		return serverSocket;

	}


}

bool ConnectionsService::TryStart()
{
	if (!connections_internal::TryStartSocketLib())
	{
		return false;
	}

	SOCKET serverSocket = connections_internal::TryBindServerSocketAndListenIt(_port.c_str());

	_serverSocket = serverSocket;

	return serverSocket != INVALID_SOCKET;
}


void RemoveFromAuthorized(SOCKET socket, std::vector<SOCKET>& authorizedClients)
{
	authorizedClients.erase(std::remove(authorizedClients.begin(), authorizedClients.end(), socket), authorizedClients.end());
}

bool ClientIsAuthorized(SOCKET socket, const std::vector<SOCKET>& authorizedClients)
{
	return std::find(authorizedClients.begin(), authorizedClients.end(), socket) != authorizedClients.end();
}

std::vector<unsigned>::iterator RemoveClientAndGetNextIterator(SOCKET client, std::vector<SOCKET>& clients)
{
	auto iterator = std::find(clients.begin(), clients.end(), client);

	if (iterator != clients.end())
	{
		return clients.erase(iterator);
	}

	return clients.end();
}

void ConnectionsService::StartListenConnections()
{
	while (true)
	{
		/*
		 * Попытка принять сокет клиента
		 * Поскольку режим сокета неблокирующий,
		 * то результат функции accept вернет INVALID_SOCKET
		 * в том, случае, если в очереди соединений нет сокетов,
		 * желающих подключиться
		 * 
		 */
		SOCKET clientSocket = accept(_serverSocket, nullptr, nullptr);

		if (clientSocket != INVALID_SOCKET) {
			_clients.push_back(clientSocket);
		}

		auto clientIterator = _clients.begin();


		while (clientIterator != _clients.end())
		{

			/*
			 * Функция recv читает данные с клиента, записывая их в буфер, фиксированной длины
			 * Возвращаемый результат - число записанных байт в буфер 
			 *
			 * Если это число < 0 - произошла ошибка при чтении данных
			 *	Для неблокирующего сокета - ошибка WSAEWOULDBLOCK (нет данных в очереди, клиент не отправил данные)	является допустимой
			 *
			 * Если это число = 0 - признак успешного отключения от сервера 
			 */


			SOCKET client = *clientIterator;

			int bytesReceived = recv(client, _buffer, _bufferSize, 0);

			if (bytesReceived < 0)
			{

				int errorCode = WSAGetLastError();

				if (errorCode == WSAEWOULDBLOCK)
				{
					++clientIterator;
					continue;
				}

				if (errorCode != WSAESHUTDOWN)
				{

					//std::cerr << "Ошибка при обработке данных с клиента " << client << std::endl;
					//std::cerr << "Клиент будет удалён " << std::endl;
					//std::cerr << "Сообщение ошибки: " << gai_strerrorA(errorCode) << std::endl;


					clientIterator = RemoveClientAndGetNextIterator(client, _clients);
					RemoveFromAuthorized(client, _authorizedClients);

					closesocket(client);


				}

			}

			else if (bytesReceived == 0)
			{

				//std::cerr << "Клиент " << client << " отключился" << std::endl;

				clientIterator = RemoveClientAndGetNextIterator(client, _clients);
				RemoveFromAuthorized(client, _authorizedClients);

				closesocket(client);

			} else
			{
				auto clientRequestType = static_cast<ClientRequestType>(_buffer[0]);


				switch (clientRequestType) {

				case ClientRequestType::AUTH:
				{
					if (ClientIsAuthorized(client, _authorizedClients))
					{
						_buffer[0] = static_cast<char>(ServerResponseType::IS_ALREADY_AUTH_ERR);
						send(client, _buffer, 1, 0);
						break;
					}

					//int bytesReceived = recv(client, _buffer, _bufferSize, 0);
					//
					//if (bytesReceived <= 0)
					//{
					//	clientIterator = RemoveClientAndGetNextIterator(client, _clients);
					//	RemoveFromAuthorized(client, _authorizedClients);
					//
					//	closesocket(client);
					//
					//	continue;
					//}

					auto authData = DoctorAuthDataConverter::Build(std::string(_buffer + 1, _buffer + bytesReceived));

					const Doctor* doctor = _repository->Authorize(authData);

					if (doctor == nullptr)
					{
						_buffer[0] = static_cast<char>(ServerResponseType::WRONG_AUTH_ERR);
						send(client, _buffer, 1, 0);
						break;
					}

					DoctorPersonalData personalData = doctor->GetPersonal();
					std::string preparedPersonalData = DoctorPersonalDataConverter::Convert(personalData);

					_buffer[0] = static_cast<char>(ServerResponseType::AUTH_OK);
					std::copy(preparedPersonalData.begin(), preparedPersonalData.end(), _buffer + 1);
					_buffer[preparedPersonalData.size() + 1] = '\0';

					send(client, _buffer, 1 + preparedPersonalData.size() + 1, 0);
					_authorizedClients.push_back(client);

					break;
				}

				case ClientRequestType::GET_PATIENT:
					{

						if (!ClientIsAuthorized(client, _authorizedClients))
						{
							_buffer[0] = static_cast<char>(ServerResponseType::NOT_AUTH_ERR);
							send(client, _buffer, 1, 0);
							break;
						}

						if (_repository->IsEmpty())
						{
							_buffer[0] = static_cast<char>(ServerResponseType::QUEUE_IS_EMPTY_ERR);
							send(client, _buffer, 1, 0);
						} else
						{

							auto patient = _repository->Pop();

							auto patientAsJson = _converter.Convert(patient);

							if (patientAsJson.length() + 1 > _bufferSize - 1)
							{
								auto allocBuffer = std::make_unique<char>(1 + patientAsJson.length() + 1);

								allocBuffer.get()[0] = static_cast<char>(ServerResponseType::GET_PATIENT_OK);

								std::copy(patientAsJson.begin(), patientAsJson.end(), allocBuffer.get() + 1);
								allocBuffer.get()[1 + patientAsJson.length()] = '\0';

								send(client, allocBuffer.get(), 1 + patientAsJson.length() + 1, 0);
							} else
							{
								_buffer[0] = static_cast<char>(ServerResponseType::GET_PATIENT_OK);

								std::copy(patientAsJson.begin(), patientAsJson.end(), _buffer + 1);
								_buffer[1 + patientAsJson.length()] = '\0';

								send(client, _buffer, 1 + patientAsJson.length() + 1, 0);
							}

						}
						break;
					}

				case ClientRequestType::LOGOUT:
					{

						clientIterator = RemoveClientAndGetNextIterator(client, _clients);
						RemoveFromAuthorized(client, _authorizedClients);

						closesocket(client);

						break;
					}

				}
			}
		}
	}
}

	/*SOCKET clientSocket = INVALID_SOCKET;

	clientSocket = accept(serverSocket, nullptr, nullptr);

	if (clientSocket == INVALID_SOCKET) {
		closesocket(listenSocket);
		WSACleanup();

		throw std::logic_error("accept failed: " + std::to_string(WSAGetLastError()));
	}




	char buffer[10];

	int bytesReceived;

	do
	{

		recv

			bytesReceived = recv(clientSocket, buffer, 10, 0);

		if (bytesReceived == 0)
		{
			std::cout << "Connection close" << std::endl;
			//Соединение с клиентом была закрыто, успешно (из вне, самим клиентом??)
		} else if (bytesReceived > 0)
		{
			std::cout << "message =  " << std::string(buffer, buffer + bytesReceived) << std::endl;
		} else
		{
			closesocket(clientSocket);
			WSACleanup();
			throw std::logic_error("recv failed: " + std::to_string(WSAGetLastError()));
		}

	} while (bytesReceived > 0);

	resultCode = shutdown(clientSocket, SD_SEND);

	if (resultCode != 0)
	{
		closesocket(clientSocket);
		WSACleanup();
		throw std::logic_error("shutdown failed: " + std::to_string(WSAGetLastError()));

	} */
