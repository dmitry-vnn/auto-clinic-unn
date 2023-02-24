#include <string>
#include <iostream>

#include "json.hpp"

#pragma once

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
	);



private:
	Type _type;

public:
	State(Type type) : _type(type) {};
	State(const State& state) : _type(state._type) {};
	State& operator=(const State& type);

	std::string ToString() const;
	Type GetType() { return _type;  };


public:
	bool operator==(const State& state) const { return _type == state._type; };
	bool operator<(const State& state) const { return _type < state._type; };

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
	Patient() : _state(State::NORMAL), _type(Type::PATIENT) {}; //requirement for nhlohmann:json parsing

	Patient(const std::string& first_name, const std::string& last_name, const std::string& patronymic = "", State state = State::NORMAL);

	Patient(const Patient& patient);
	Patient& operator=(const Patient& patient);

public:
	std::string GetFirstName() const { return _first_name; };
	std::string GetLastName() const { return _last_name; };
	std::string GetPatronymic() const { return _patronymic; };

	State GetState() const { return _state; };
	Type GetType() const { return _type; };

public:
	bool operator==(const Patient& patient) const;
	bool operator<(const Patient& patient) const { return _state < patient._state; };
	//bool operator<=(const Patient& patient) const { this->operator<(patient) || this->operator==(patient); };

public:
	friend std::ostream& operator<<(std::ostream& stream, const Patient& patient);

};


class MajorPatient : public Patient {
	
private:
	size_t _money;

public:

	MajorPatient() : Patient(), _money(0) { _type = Type::MAJOR_PATIENT; }; //requirement for nhlohmann:json parsing

	MajorPatient(size_t money, const std::string& first_name, const std::string& last_name, const std::string& patronymic, State state = State::NORMAL)
		: Patient(first_name, last_name, patronymic, state), _money(money) { _type = Type::MAJOR_PATIENT;};

	MajorPatient(const MajorPatient& patient) : Patient(patient), _money(patient._money) {};
	MajorPatient& operator=(const MajorPatient& patient);

	size_t GetMoney() const { return _money; }

public:
	bool operator==(const MajorPatient& patient) const { return this == &patient ? true : _money != patient._money ? false : Patient::operator==(patient);  };
	bool operator<(const MajorPatient& patient) const { return _money < patient._money ? true : patient._money < _money ? false : Patient::operator<(patient); };

public:
	friend std::ostream& operator<<(std::ostream& stream, const MajorPatient& patient);

public:
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MajorPatient, _money)
};

void to_json(nlohmann::json& json, const Patient& patient);
void from_json(const nlohmann::json& json, Patient& patient);

void to_json(nlohmann::json& json, const MajorPatient& patient);
void from_json(const nlohmann::json& json, MajorPatient& patient);