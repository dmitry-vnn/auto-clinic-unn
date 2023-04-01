#pragma once

#include <iostream>
#include <thread>

#include "repository.h"

class PatientReceptionUI
{

private:

	Repository* _repository;

public:
	explicit PatientReceptionUI(Repository* repository): _repository(repository) {}

	PatientReceptionUI(const PatientReceptionUI& other) = delete;
	PatientReceptionUI& operator=(const PatientReceptionUI& other) = delete;

public:

	void Start();
};
