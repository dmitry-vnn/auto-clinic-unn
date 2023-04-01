#include "reception_ui.h"

#include "client_service.h"


#define print std::cout
#define ln std::endl

#define scan std::cin


#define printerr print << "\x1B[91mОШИБКА\033[0m: "
#define printpar(x) print << "	\x1B[33m" << (x) << "\033[0m) "

//#define define println(x) std::cout << (x) << std::endl 


namespace reception_internals
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


	std::unique_ptr<const Patient> TryCreatePatientOnHeap()
	{
		while (true)
		{
			print << "(для отмены, введите символ 'c')" << ln;

			std::string firstName;

			while (true)
			{
				print << "Имя пациента: ";

				std::getline(scan, firstName);

				if (firstName == "c")
				{
					return nullptr;
				}

				if (firstName.empty())
				{
					continue;
				}

				break;

			}

			std::string lastName;

			while (true)
			{
				print << "Фамилия пациента: ";

				std::getline(scan, lastName);

				if (lastName == "c")
				{
					return nullptr;
				}

				if (lastName.empty())
				{
					continue;
				}

				break;

			}

			std::string patronymic;

			while (true)
			{
				print << "Отчество пациента (oпционально): ";

				std::getline(scan, patronymic);

				if (patronymic == "c")
				{
					return nullptr;
				}

				if (patronymic.empty())
				{
					break;
				}

				break;

			}


			State state = State::NORMAL;

			while (true)
			{

				print << "Пункты состояния пациента: " << ln;
				printpar(1) << "В норме (поумолчанию)" << ln;
				printpar(2) << "Стабильное" << ln;
				printpar(3) << "Тяжёлое" << ln;
			
				print << "Введите пункт состояния (опционально): ";

				std::string stateAsString;

				std::getline(scan, stateAsString);

				if (stateAsString == "c")
				{
					return nullptr;
				}

				if (stateAsString.empty() || stateAsString == "1")
				{
					break;
				}

				if (stateAsString == "2")
				{
					state = State::MEDIUM;
					break;
				}

				if (stateAsString == "3")
				{
					state = State::CRITICAL;
					break;
				}

				printerr << "Введённый вами пункт состояния недействителен! Повторите попытку" << ln;

			}


			bool isMajor = false;

			while (true)
			{


				print << "Пункты статусов пациента: " << ln;
				printpar(1) << "Обычный (поумолчанию)" << ln;
				printpar(2) << "Привилегированный" << ln;
	
				print << "Введите пункт статуса (опционально): ";

				std::string statusAsStr;

				std::getline(scan, statusAsStr);

				if (statusAsStr == "c")
				{
					return nullptr;
				}

				if (statusAsStr.empty() || statusAsStr == "1")
				{
					break;
				}

				if (statusAsStr == "2")
				{
					isMajor = true;
					break;
				}

				printerr << "Введённый вами пункт статуса неверен! Повторите попытку" << ln;
			}


			if (isMajor)
			{

				while (true)
				{
					std::string moneyAsStr;

					print << "Денег: ";

					std::getline(scan, moneyAsStr);

					if (moneyAsStr == "c")
					{
						return nullptr;
					}

					size_t money = ToSizeTOrElse(moneyAsStr, 0);

					if (money == 0)
					{
						printerr << "Введённый вами сумма не является натуральным числом! Повторите попытку" << ln;
						continue;
					}

					return std::make_unique<MajorPatient>(money, firstName, lastName, patronymic, state);
				}
			}

			return std::make_unique<Patient>(firstName, lastName, patronymic, state);
		}
	}
}


void PatientReceptionUI::Start()
{

	print << "Запуск приёмной..." << ln;
	print << "Приёмная успешно запущена!" << ln;


	while (true)
	{
		print << "Пункт действия:" << ln;
		//print << ln;
		printpar(1) << "Добавить пациента" << ln;
		printpar(2) << "Число записавшихся пациентов" << ln;
		//print << ln;
		print << "Введите пункт действия: ";

		std::string actionAsStr;

		std::getline(scan, actionAsStr);

		if (actionAsStr == "1")
		{

			auto patient = reception_internals::TryCreatePatientOnHeap();

			if (patient == nullptr)
			{
				continue;
			}

			_repository->Push(std::move(patient));


			print << "Пациент \x1B[32mуспешно\033[0m записан на приём" << ln;
			continue;
		}

		if (actionAsStr == "2")
		{
			print << "Пациентов записано: " << _repository->Size() << ln;
			continue;
		}

		printerr << "Введённый вами пункт действия некорректен! Повторите попытку" << ln;

	}



}
