#include "doctor.h"

void to_json(nlohmann::json& json, const DoctorAuthData& auth)
{
	json["login"] = auth.login;
	json["password"] = auth.password;
}

void from_json(const nlohmann::json& json, DoctorAuthData& auth)
{
	auth.password = json["password"];
	auth.login = json["login"];
}

void to_json(nlohmann::json& json, const DoctorPersonalData& personal)
{
	json["firstName"] = personal.firstName;
	json["lastName"] = personal.lastName;
}

void from_json(const nlohmann::json& json, DoctorPersonalData& personal)
{
	personal.firstName = json["firstName"];
	personal.lastName = json["lastName"];
}
