#pragma once
#include "client_service.h"

class DoctorUI
{

private:
	ClientService* _clientService;
	PatientConverter _converter;

public:
	explicit DoctorUI(ClientService* clientService) : _clientService(clientService), _converter() {}

	DoctorUI(const DoctorUI& other) = delete;
	DoctorUI& operator=(const DoctorUI& other) = delete;

public:
	void Start();
};
