#pragma once

#include <string>
#include "json.hpp"

struct DoctorAuthData
{
	std::string login;
	std::string password;

	DoctorAuthData(std::string login, std::string password):
		login(std::move(login)), password(std::move(password))	{}

	DoctorAuthData() : DoctorAuthData("", "") {}


	friend bool operator==(const DoctorAuthData& lhs, const DoctorAuthData& rhs)
	{
		return lhs.login == rhs.login
			&& lhs.password == rhs.password;
	}

	friend bool operator!=(const DoctorAuthData& lhs, const DoctorAuthData& rhs)
	{
		return !(lhs == rhs);
	}
};


void to_json(nlohmann::json& json, const DoctorAuthData& auth);
void from_json(const nlohmann::json& json, DoctorAuthData& auth);

struct DoctorPersonalData
{
	std::string firstName;
	std::string lastName;

	DoctorPersonalData(std::string firstName, std::string lastName):
		firstName(std::move(firstName)), lastName(std::move(lastName))	{}


	DoctorPersonalData() : DoctorPersonalData("", "") {}


	friend bool operator==(const DoctorPersonalData& lhs, const DoctorPersonalData& rhs)
	{
		return lhs.firstName == rhs.firstName
			&& lhs.lastName == rhs.lastName;
	}

	friend bool operator!=(const DoctorPersonalData& lhs, const DoctorPersonalData& rhs)
	{
		return !(lhs == rhs);
	}
};

void to_json(nlohmann::json& json, const DoctorPersonalData& personal);
void from_json(const nlohmann::json& json, DoctorPersonalData& personal);


class Doctor
{

private:
	DoctorAuthData _auth;
	DoctorPersonalData _personal;


public:

	Doctor(std::string firstName, std::string lastName, std::string login, std::string password):
		_auth{ std::move(login), std::move(password) },
		_personal{ std::move(firstName), std::move(lastName) }	{}

	Doctor(const Doctor& other) = default;
	Doctor& operator=(const Doctor& other) = default;

	DoctorPersonalData GetPersonal() const { return _personal; }

	bool CanAuth(const DoctorAuthData& auth) const { return _auth == auth; }
};

class DoctorAuthDataConverter
{

public:
	static std::string Convert(const DoctorAuthData& auth)
	{
		return nlohmann::json(auth).dump();
	}

	static DoctorAuthData Build(const std::string& str)
	{
		return nlohmann::json::parse(str);
	}

};


class DoctorPersonalDataConverter {

public:
	static std::string Convert(const DoctorPersonalData& auth)
	{
		return nlohmann::json(auth).dump();
	}

	static DoctorPersonalData Build(const std::string& str)
	{
		return nlohmann::json::parse(str);
	}

};
