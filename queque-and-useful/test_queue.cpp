#ifdef _DEBUG

#include <gtest/gtest.h>
#include "queue.h"

TEST(queue, majorr)
{
	SUCCEED();
}

TEST(queue, pat)
{
	Queue<Patient*, Order::ORDERED> q;

	Patient
		p1("Ivan", "Petrov"),
		p2("Peter", "Parker", State::CRITICAL),
		p3("Denis", "Denisov", State::CRITICAL),
		p4("Abdula", "Al Ahmed", State::MEDIUM);

	MajorPatient
		mp1(15000, "Lyoha", "Bumer");

	q += &p1;
	q += &p2;
	q += &p3;
	q += &p4;

	q += &mp1;


	// std::cout << p1.GetFirstName() << std::endl;
	// std::cout << p1.GetLastName() << std::endl;


	while (!q.IsEmpty())
	{
		const auto* p = q.Pop();

		std::cout << p->GetFirstName() << std::endl;
		std::cout << p->GetLastName() << std::endl;

		std::cout << std::endl;
	}

	//EXPECT_EQ(q.Pop(), mp1);
	//EXPECT_EQ(q.Pop(), p2);
	//EXPECT_EQ(q.Pop(), p3);
	//EXPECT_EQ(q.Pop(), p4);
	//EXPECT_EQ(q.Pop(), p1);
	

}

template<class T>
bool Compare(const std::pair<size_t, T>& e1, const std::pair<size_t, T>& e2) {
	if (e1.second < e2.second) {
		return false;
	}

	if (e2.second < e1.second) {
		return true;
	}

	return e1.first < e2.first;

}

template<class T>
size_t SearchInCyclicBuffer(const std::pair<size_t, T>* data, size_t size, size_t cyclicStartIndex, size_t elementsCount, const std::pair<size_t, T>& element) {

	int startIndex = 0;
	int endIndex = elementsCount - 1;

	do {
		size_t midIndex = (startIndex + endIndex) / 2;
		
		size_t midIndexCyclic = (midIndex + cyclicStartIndex) % size;

		//buffer contains only unique elements !! - pair for different places in queue, as idea

		if (Compare(element, data[midIndexCyclic])) {
			endIndex = midIndex - 1;
		}
		else {
			startIndex = midIndex + 1;
		}

	}

	while (endIndex >= startIndex);

	return startIndex; //always >= 0

}

TEST(queue, search) {

	std::pair<size_t, int> data[] { 
		{1, 19}, {2, 8}, {3, 4}, {4, 4}, {6, 2}, {7, 0}
	};

	size_t size = sizeof(data) / sizeof(*data);

	size_t insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, 4 }
	);


	EXPECT_EQ(insertIndex, 4);
	
	insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, 8 }
	);
	
	EXPECT_EQ(insertIndex, 2);
	
	insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, 5 }
	);
	
	EXPECT_EQ(insertIndex, 2);
	
	insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, 10 }
	);
	
	EXPECT_EQ(insertIndex, 1);
	
	insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, 19 }
	);
	
	EXPECT_EQ(insertIndex, 1);


	insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, 25 }
	);

	EXPECT_EQ(insertIndex, 0);

	insertIndex = SearchInCyclicBuffer(data, size, 0, size,
		std::pair<size_t, int> { 99, -3 }
	);

	EXPECT_EQ(insertIndex, size);

}

TEST(queue, sorted) {

	Queue<int, Order::ORDERED> q;

	q.Push(5);
	q.Push(3);
	q.Push(9);
	q.Push(11);
	q.Push(19);
	q.Push(3);
	q.Push(8);
	q.Push(9);
	q.Push(7);

	std::cout << q << std::endl;

	EXPECT_EQ(q.Pop(), 19);
	EXPECT_EQ(q.Pop(), 11);
	EXPECT_EQ(q.Pop(), 9);
	EXPECT_EQ(q.Pop(), 9);
	EXPECT_EQ(q.Pop(), 8);
	EXPECT_EQ(q.Pop(), 7);
	EXPECT_EQ(q.Pop(), 5);
	EXPECT_EQ(q.Pop(), 3);
	EXPECT_EQ(q.Pop(), 3);

	std::cout << q << std::endl;

	q.Push(59);

	std::cout << q << std::endl;

	q.Push(3);

	std::cout << q << std::endl;

	q.Push(59);

	std::cout << q << std::endl;

	q.Push(59);

	std::cout << q << std::endl;
	//q.Pop();
	//q.Pop();
	q.Push(9);

	std::cout << q << std::endl;

	EXPECT_EQ(q.Pop(), 59);
	EXPECT_EQ(q.Pop(), 59);
	EXPECT_EQ(q.Pop(), 59);
	EXPECT_EQ(q.Pop(), 9);
	EXPECT_EQ(q.Pop(), 3);

		std::cout << q << std::endl;

}

TEST(queue, order) {


	auto list = { 1, 8, 5, 6, 2, 3, 50, -5, 5, 35 };

	Queue<int, Order::ORDERED> q(list);
	
	std::cout << q;
	
}

TEST(queue, init1) {

	std::initializer_list<int> list{5, 1, 4, 20, 95, 1, 5, 1, 5, 16, 0, -5, 1};

	Queue<int, Order::UNORDERED> q(list);

	auto iter = list.begin();
	
	while (iter != list.end())
	{
		ASSERT_EQ(q.Pop(), *(iter++));
	}
}

TEST(queue, init2) {

	Queue<int> q({ 5, 1, 4 });

	q.Push(q.Pop());
	q.Push(q.Pop());
	q.Push(q.Pop());

	ASSERT_EQ(q.Pop(), 5);
	ASSERT_EQ(q.Pop(), 1);
	ASSERT_EQ(q.Pop(), 4);

}

TEST(queue, init3) {

	Queue<int> q(3);

	q.Push(9);
	q.Push(10);
	q.Push(11);

	q.Pop();
	q.Push(5);
	q.Pop();
	q.Pop();
	q.Push(52);

	ASSERT_EQ(q.Pop(), 5);
	ASSERT_EQ(q.Pop(), 52);

}

TEST(queue, init4) {

	Queue<int> q1{ 10, 95, 105 };
	
	q1.Pop();
	q1.Pop();
	q1.Push(200);
	q1.Push(11);

}

#endif