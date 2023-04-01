#include "doctor_ui.h"

#include "doctor.h"

#define print std::cout

#define println(x) std::cout << (x) << std::endl 

#define ln std::endl
#define scan std::cin

#define printerr print << "\x1B[91mОШИБКА\033[0m: "
#define printpar(x) print << "	\x1B[33m" << (x) << "\033[0m) "

namespace doctor_internals
{
	size_t ToSizeTOrElse(const std::string& str, size_t orElse)
	{
		try {
			int i = std::stoi(str);
			return i < 0 ? orElse : i;
		} catch (const std::exception&)
		{
			return orElse;
		}
	}






	void ServePatient(std::unique_ptr<Patient> patient)
	{
		print << "Вы обслуживаете пациента: " << ln;

		print << " ФИО: "  << "\x1B[35m" <<patient->GetLastName() << " " << patient->GetFirstName() << " " << patient->GetPatronymic() << "\033[0m" << ln;

		print << " Состояние: ";

		switch (patient->GetState().GetType()) {

			case State::NORMAL:
				print << "\x1B[32mОтличное\033[0m" << ln;
				break;

			case State::MEDIUM:
				print << "\x1B[33mСтабильное\033[0m" << ln;
				break;

			case State::CRITICAL:
				print << "\x1B[91mКритическое\033[0m" << ln;
				break;
		}

		print << " Класс: ";

		if (patient->GetType() == PATIENT)
		{
			print << "\x1B[36mОБЫЧНЫЙ\033[0m" << ln;
		} else
		{
			print << "\x1B[36mПРИВИЛЕГИРОВАННЫЙ\033[0m" << ln;

			const auto& majorPatient = dynamic_cast<MajorPatient*>(patient.get());

			print << " Состояние: " << majorPatient->GetMoney() << " Б.Е" << ln;

		}

		//Типа обслуживание
		Sleep(2000);
	}
}


void DoctorUI::Start()
{

	while (true)
	{
		std::string login;
		while (true)
		{
			print << "Логин: ";
			std::getline(scan, login);

			if (!login.empty())
			{
				break;
			}
		}

		std::string password;
		while (true)
		{
			print << "Пароль: ";
			std::getline(scan, password);

			if (!password.empty())
			{
				break;
			}
		}


		DoctorPersonalData personalData;

		try
		{
			personalData = DoctorPersonalDataConverter::Build(
				_clientService->AuthorizeAndGetPersonalData(
						DoctorAuthDataConverter::Convert(DoctorAuthData(login, password))
					)
			);

		} catch (const BadResponse& error)
		{
			auto response = error.GetResponseType();

			if (response == ServerResponseType::IS_ALREADY_AUTH_ERR)
			{
				printerr << "Вы уже авторизованы!" << std::endl;
				continue;
			}

			if (response == ServerResponseType::WRONG_AUTH_ERR)
			{
				printerr << "Неверный логин или пароль" << std::endl;
				continue;
			}

			continue;

		}

		print << "Здравствуйте " << personalData.lastName << " " << personalData.firstName << ln;


		while (true)
		{

			print << "Пункт действия:" << ln;
	
			printpar(1) << "Запросить пациета" << ln;
			printpar(2) << "Выйти" << ln;

			print << "Введите пункт действия: ";

			std::string actionAsStr;

			std::getline(scan, actionAsStr);

			if (actionAsStr == "1")
			{
				try
				{
					const auto& patientAsString = _clientService->GetPatientAsString();

					auto patient = PatientConverter::Build(patientAsString);

					doctor_internals::ServePatient(std::move(patient));

				} catch (const BadResponse& error)
				{
					auto response = error.GetResponseType();

					if (response == ServerResponseType::NOT_AUTH_ERR)
					{
						printerr << "Вы ещё не авторизованы" << std::endl;
						continue;
					}

					if (response == ServerResponseType::QUEUE_IS_EMPTY_ERR)
					{
						printerr << "Пока ещё нет ни одного записавшегося пациента" << std::endl;
						continue;
					}


				} catch (const PatientConvertError& err)
				{
					printerr << "Не удалось распознать данные пациента, повторите попытку!" << ln;
					print << "Причина: " << err.what() << ln;
				}

				continue;
			}

			if (actionAsStr == "2")
			{
				_clientService->Logout();
				print << "До скорых встреч!" << ln;
				return;
			}

			printerr << "Введённый вами пункт действия некорректен! Повторите попытку" << ln;
		}
	}

}
