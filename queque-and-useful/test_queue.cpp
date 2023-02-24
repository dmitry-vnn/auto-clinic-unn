#include <gtest/gtest.h>
#include "queue.h"


template<class T>
size_t SearchInCyclicBuffer(const std::pair<T, size_t>* buffer, size_t bufferSize, size_t cyclicStartIndex, size_t elementsCount, const T& element) {

	size_t startIndex = 0;
	size_t endIndex = elementsCount - 1;

	do {
		size_t midIndex = (startIndex + endIndex) / 2 + 1;
		
		size_t midIndexCyclic = (midIndex + cyclicStartIndex) % bufferSize;

		if (element < buffer[midIndexCyclic]) {
			startIndex = midIndex + 1;
		}
		else if (buffer[midIndexCyclic] < element) {
			endIndex = midIndex - 1;
		}

		return midIndex;

	}

	while (endIndex >= startIndex);



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