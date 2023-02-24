#include <gtest/gtest.h>

#include "json.hpp"

#include "patient.h"

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