#include "queue.h"
#ifdef _DEBUG

#include <gtest/gtest.h>

#include "json.hpp"

#include "patient.h"

TEST(PPP, ccc)
{

	Queue<std::unique_ptr<Patient>, Order::ORDERED> queue;
	
	queue.Push(std::make_unique<Patient>("Dm", "Vn", "Ig"));
	
	// std::unique_ptr<Patient> patient = queue.Pop();
	std::unique_ptr<Patient> patient = queue.Pop();




	SUCCEED();

}

TEST(patient, convert)
{


	Patient p1("Paul", "Walker", "-", State::MEDIUM);

	PatientConverter converter;

	std::string str = converter.Convert(std::make_unique<Patient>(p1));

	std::unique_ptr<Patient> p2 = converter.Build(str);

	MajorPatient m1(10, "Lyoha", "Bander");

	std::unique_ptr<Patient> m2 = converter.Build(converter.Convert(std::make_unique<MajorPatient>(m1)));

	EXPECT_TRUE(m2->operator==(m1));

}

TEST(patient, json) {

	Patient p("A", "B", "C", State::MEDIUM);

	nlohmann::json j = p;

	Patient fromJsonP = j.get<Patient>();

	EXPECT_EQ(p, fromJsonP);

}

TEST(major_patient, json) {

	MajorPatient p(10, "A", "B", "C", State::MEDIUM);

	nlohmann::json j = p;

	MajorPatient fromJsonP = j.get<MajorPatient>();

	EXPECT_EQ(p, fromJsonP);

}

TEST(patient, compare) {

	Patient p1("A", "B"), p2("C", "D");

	//p1 <= p2;

}

#endif