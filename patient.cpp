#include "patient.h"

State& State::operator=(const State& state)
{
    if (this != &state) {
        _type = state._type;
    }

    return *this;
}

std::string State::ToString() const
{
    switch (_type)
    {
        case State::NORMAL: {
            return "NORMAL";
            break;
        }
        case State::MEDIUM: {
            return "MEDIUM";
            break;
        }
        case State::CRITICAL: {
            return "CRITICAL";
            break;
        }
        default: {
            throw "Not handling state";
        }
    }
}


std::ostream& operator<<(std::ostream& stream, const State& state)
{
    stream << state.ToString();
    stream << "(";
    stream << state._type;
    stream << ")";

    return stream;
}


Patient::Patient(const std::string& first_name, const std::string& last_name, const std::string& patronymic, State state) :
    _first_name(first_name),
    _last_name(last_name),
    _patronymic(patronymic),
    _state(state),
    _type(Type::PATIENT)
{
}

Patient::Patient(const Patient& patient) :
    _first_name(patient._first_name),
    _last_name(patient._last_name),
    _patronymic(patient._patronymic),
    _state(patient._state),
    _type(patient._type)
{
}

Patient& Patient::operator=(const Patient& patient)
{
    if (this != &patient) {
        _first_name = patient._first_name,
        _last_name = patient._last_name,
        _patronymic = patient._patronymic,
        _state = patient._state;
        _type = patient._type;
    }

    return *this;
}


bool Patient::operator==(const Patient& patient) const
{
    return this == &patient || (

        _first_name == patient._first_name &&
        _last_name == patient._last_name &&
        _patronymic == patient._patronymic &&
        _state == patient._state &&
        _type == patient._type
            
    );
}

std::ostream& operator<<(std::ostream& stream, const Patient& patient)
{

    stream << "first_name = \"";
    stream << patient._first_name;
    stream << "\"";

    stream << ", last_name = \"";
    stream << patient._last_name;
    stream << "\"";

    stream << ", patronymic = ";
    stream << patient._patronymic;

    stream << ", state = ";
    stream << patient._state;

    return stream;
}


std::ostream& operator<<(std::ostream& stream, const MajorPatient& majorPatient)
{
    const Patient& patient = majorPatient;
    
    stream << patient;

    stream << ", money = ";
    stream << majorPatient._money;

    return stream;

}

MajorPatient& MajorPatient::operator=(const MajorPatient& patient)
{
    if (this != &patient) {
        _money = patient._money;
        Patient::operator=(patient);
    }

    return *this;
}

void to_json(nlohmann::json& json, const Patient& patient)
{
    json = {
        {"firstName", patient.GetFirstName()},
        {"lastName", patient.GetLastName()},
        {"patronymic", patient.GetPatronymic()},
        {"state", patient.GetState().GetType()},
        {"type", patient.GetType()},
    };

}

void from_json(const nlohmann::json& json, Patient& patient)
{
    patient = Patient(json["firstName"], json["lastName"], json["patronymic"], State(json["state"]));
}

void to_json(nlohmann::json& json, const MajorPatient& patient)
{

    to_json(json, *static_cast<const Patient*>(&patient));
    json["money"] = patient.GetMoney();

}

void from_json(const nlohmann::json& json, MajorPatient& patient)
{
    from_json(json, *static_cast<Patient*>(&patient));
    patient._money = json["money"];
}
