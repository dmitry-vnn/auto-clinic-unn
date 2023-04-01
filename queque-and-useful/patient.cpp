#include "patient.h"

#include <utility>

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
        case NORMAL: {
            return "NORMAL";
        }
        case MEDIUM: {
            return "MEDIUM";
        }
        case CRITICAL: {
            return "CRITICAL";
        }
    }

    throw std::logic_error("State not handling");
}


std::ostream& operator<<(std::ostream& stream, const State& state)
{
    stream << state.ToString();
    stream << "(";
    stream << state._type;
    stream << ")";

    return stream;
}


Patient::Patient(std::string first_name, std::string last_name, std::string patronymic, const State state) :
    _first_name(std::move(first_name)),
    _last_name(std::move(last_name)),
    _patronymic(std::move(patronymic)),
    _state(state),
    _type(PATIENT)
{
}

Patient& Patient::operator=(const Patient& patient)
{
    if (this != &patient) {
        _first_name = patient._first_name;
        _last_name = patient._last_name;
        _patronymic = patient._patronymic;
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

bool Patient::operator<(const Patient& patient) const
{
    if (patient.GetType() == MAJOR_PATIENT)
    {
        const auto& major = dynamic_cast<const MajorPatient&>(patient);
        return !(major.operator<(*this));
    }
    return _state < patient._state;
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

bool MajorPatient::operator==(const Patient& patient) const
{
	if (patient.GetType() == MAJOR_PATIENT)
	{
        const auto& major = dynamic_cast<const MajorPatient&>(patient);
        if (major.GetMoney() != GetMoney())
        {
            return false;
        }
	}

    return Patient::operator==(patient);
}

bool MajorPatient::operator<(const Patient& patient) const
{
    if (patient.GetType() == MAJOR_PATIENT) {

        size_t money = dynamic_cast<const MajorPatient&>(patient).GetMoney();

        if (GetMoney() == money)
        {
            return GetState() < patient.GetState();
        }

        return GetMoney() < money;
    }

    return GetState() < patient.GetState();
}

std::string PatientConverter::Convert(const std::unique_ptr<const Patient>& patient)
{
    return nlohmann::json(std::move(patient)).dump();
}

std::unique_ptr<Patient> PatientConverter::Build(const std::string& str)
{
    const auto& json = nlohmann::json::parse(str);

    if (json["type"] == MAJOR_PATIENT)
    {
        const MajorPatient& p = json;
        return std::make_unique<MajorPatient>(p);
    }

    const Patient& p = json;
    return std::make_unique<Patient>(p);
}

void to_json(nlohmann::json& json, const Patient& patient)
{
    try
    {
    	json = {
            {"firstName", patient.GetFirstName()},
            {"lastName", patient.GetLastName()},
            {"patronymic", patient.GetPatronymic()},
            {"state", patient.GetState().GetType()},
            {"type", patient.GetType()}
        };

        if (patient.GetType() == MAJOR_PATIENT) {
            const auto& major = dynamic_cast<const MajorPatient&>(patient);
            json["money"] = major.GetMoney();
        }

    } catch (const nlohmann::json::exception& exception)
    {
        throw PatientConvertError(exception.what());
    }

}

void from_json(const nlohmann::json& json, Patient& patient)
{
	try
	{

        patient = Patient(json["firstName"], json["lastName"], json["patronymic"], State(json["state"]));

        if (json["type"] == MAJOR_PATIENT) {
            auto& major = dynamic_cast<MajorPatient&>(patient);

            major._money = json["money"];
            major._type = MAJOR_PATIENT;
        }

	}
    catch (const nlohmann::json::exception& exception)
    {
        throw PatientConvertError(exception.what());
    }

}

void to_json(nlohmann::json& json, const std::unique_ptr<const Patient>& patient)
{
    if (patient->GetType() == MAJOR_PATIENT)
    {
        to_json(json, *dynamic_cast<const MajorPatient*>(patient.get()));
    } else
    {
        to_json(json, *patient);
    }
}

