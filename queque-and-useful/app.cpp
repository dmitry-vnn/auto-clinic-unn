#include <iostream>
#include <thread>
#include <gtest/gtest.h>


#include "client_service.h"
#include "connections_service.h"
#include "doctor_ui.h"
#include "reception_ui.h"


void StartServer()
{

	std::cout << "Запуск сервера..." << std::endl;

	Repository repository( { {"Doctor", "Watson", "root", "root"} } );
	ConnectionsService service(8803, &repository);

	if (!service.TryStart())
	{
		std::cerr << "Не удалось запустить сервер. Попробуйте снова" << std::endl;
		return;
	}

	std::cout << "Сервер успешно запущен" << std::endl;

	std::thread serviceTask([&service]
	{
		std::cout << "Начинаем прослушку входящих запросов..." << std::endl;
		service.StartListenConnections();
	});

	std::thread receptionUiTask([&repository]
	{
		PatientReceptionUI receptionUi(&repository);
		receptionUi.Start();
	});

	serviceTask.join();
	receptionUiTask.join();

	std::cout << "Сервер завершил свою работу" << std::endl;

}

void StartClient()
{
	std::cout << "Запуск клиента..." << std::endl;

	ClientService client(8803, 5, 500);

	if (!client.TryStart())
	{
		std::cerr << "Не удалось запустить клиент. Попробуйте снова" << std::endl;
		return;
	}

	std::thread doctorUiTask([&client]
	{
		DoctorUI doctorUi(&client);
		doctorUi.Start();
	});

	doctorUiTask.join();

	std::cout << "Клиент завершил свою работу" << std::endl;

}


void main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	if (argc == 2 && (std::string(argv[1]) == "-cli"))
	{
		StartClient();
		return;
	}
	 
	StartServer();


	//::testing::InitGoogleTest(&argc, argv);
	//::testing::GTEST_FLAG(filter) = "PPP*";
	//RUN_ALL_TESTS();



}
