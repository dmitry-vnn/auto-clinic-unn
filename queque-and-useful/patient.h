#pragma once

#include <string>
#include <iostream>
#include <utility>

#include "json.hpp"

class State {

public:
	enum Type : int {
		NORMAL, MEDIUM, CRITICAL
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(Type,
		{
			{NORMAL, "normal"},
			{MEDIUM, "medium"},
			{CRITICAL, "critical"}
		}
	)

private:
	Type _type;

public:
	State(const Type type) : _type(type) {}
	State(const State& state) = default;
	State& operator=(const State& state);

	std::string ToString() const;
	Type GetType() const { return _type;  }

public:
	bool operator==(const State& state) const { return _type == state._type; }
	bool operator<(const State& state) const { return _type < state._type; }

public:
	friend std::ostream& operator<<(std::ostream& stream, const State& state);

};


enum Type : int {

	PATIENT, MAJOR_PATIENT

};

NLOHMANN_JSON_SERIALIZE_ENUM(Type,
	{
		{PATIENT, "patient"},
		{MAJOR_PATIENT, "majorPatient"}
	}
);

class Patient {

private:
	std::string _first_name;
	std::string _last_name;
	std::string _patronymic;

	State _state;

protected:
	Type _type;
	

public:
	Patient() : _state(State::NORMAL), _type(PATIENT) {}

	Patient(std::string first_name, std::string last_name, std::string patronymic, State state = State::NORMAL);
	Patient(std::string first_name, std::string last_name, State state = State::NORMAL) :
		Patient(std::move(first_name), std::move(last_name), "", state) {}

	Patient(const Patient& patient) = default;
	Patient& operator=(const Patient& patient);

	virtual ~Patient() = default;

public:
	std::string GetFirstName() const { return _first_name; }
	std::string GetLastName() const { return _last_name; }
	std::string GetPatronymic() const { return _patronymic; }

	State GetState() const { return _state; }
	Type GetType() const { return _type; }

public:
	virtual bool operator==(const Patient& patient) const;
	bool operator<(const Patient& patient) const;

public:
	friend std::ostream& operator<<(std::ostream& stream, const Patient& patient);
};


class MajorPatient : public Patient {
	
private:
	size_t _money;

public:

	MajorPatient() : Patient(), _money(0) { _type = MAJOR_PATIENT; } //requirement for nhlohmann:json parsing

	MajorPatient(const size_t money, std::string first_name, std::string last_name, std::string patronymic, const State state = State::NORMAL)
		: Patient(std::move(first_name), std::move(last_name), std::move(patronymic), state), _money(money) { _type = MAJOR_PATIENT;}


	MajorPatient(const size_t money, std::string first_name, std::string last_name, const State state = State::NORMAL) :
		MajorPatient(money, std::move(first_name), std::move(last_name), "", state) {}

	MajorPatient(const MajorPatient& patient) = default;
	MajorPatient& operator=(const MajorPatient& patient);

	~MajorPatient() override = default;

public:
	bool operator==(const Patient& patient) const override;
	bool operator<(const Patient& patient) const;

	size_t GetMoney() const { return _money; }

public:
	friend std::ostream& operator<<(std::ostream& stream, const MajorPatient& majorPatient);

public:
	//Allow access to fields MajorPatient and Patient classes for "from_json" function 
	friend void from_json(const nlohmann::json& json, Patient& patient);

};

class PatientConvertError : public std::exception
{
public:
	explicit PatientConvertError(char const* message): exception(message) {}
};

class PatientConverter
{

public:
	static std::string Convert(const std::unique_ptr<const Patient>& patient);
	static std::unique_ptr<Patient> Build(const std::string& str);


};

void to_json(nlohmann::json& json, const Patient& patient);
void from_json(const nlohmann::json& json, Patient& patient);

void to_json(nlohmann::json& json, const std::unique_ptr<const Patient>& patient);
//void from_json(const nlohmann::json& json, std::unique_ptr<Patient>& patient);

