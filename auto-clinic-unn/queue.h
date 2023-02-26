#pragma once

#include <iostream>
#include <algorithm>

#include "patient.h"

enum class Order {
	ORDERED,
	UNORDERED
};

template <class T, Order ORDER = Order::UNORDERED>
class Queue {
	

private:
	
	std::conditional_t<ORDER == Order::ORDERED, std::pair<size_t, T>*, T*> data;
	size_t size;

	size_t first_index;
	size_t after_last_index;

	size_t count_of_elements;

	size_t absolute_last_element_position;

public:
	explicit Queue(size_t initial_capacity = 10);
	explicit Queue(std::initializer_list<T> list);
	~Queue();

public:

	void Push(const T& element);
	inline bool IsFull() const;

	T Pop();
	inline bool IsEmpty() const;

	template <class T, Order ORDER>
	friend std::ostream& operator<<(std::ostream& stream, const Queue<T, ORDER>& queue);

public:

	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;

private:

	static bool Compare(const std::pair<size_t, T>& right, const std::pair<size_t, T>& left);
	size_t BinarySearchIndexForInsertElement(const std::pair<size_t, T>& element);

};

template<class T, Order ORDER>
inline Queue<T, ORDER>::Queue(size_t initial_capacity) :
	size(initial_capacity),
	first_index(0),
	after_last_index(0),
	count_of_elements(0),
	absolute_last_element_position(0)
{
	if constexpr (ORDER == Order::ORDERED)
		data = new std::pair<size_t, T>[size];
	else
		data = new T[size];
}

template<class T, Order ORDER>
inline Queue<T, ORDER>::Queue(std::initializer_list<T> list) :
	size(list.size()),
	first_index(0),
	after_last_index(0),
	absolute_last_element_position(0),
	count_of_elements(list.size())
{
	if constexpr (ORDER == Order::ORDERED) {
		data = new std::pair<size_t, T>[size];
		
		for (size_t i = 0; i < size; i++)
		{
			data[i] = { i, *(list.begin() + i) };
		}

		std::sort(data, data + size, Compare);
	}
	else {
		data = new T[size];
		std::copy(list.begin(), list.end(), data);
	}

}

template<class T, Order ORDER>
inline Queue<T, ORDER>::~Queue()
{
	delete[] data;
	data = nullptr;
}

template<class T, Order ORDER>
inline void Queue<T, ORDER>::Push(const T& element)
{
	if (!IsFull()) {

		if constexpr (ORDER == Order::ORDERED) {
			std::pair<size_t, T> pair = { ++absolute_last_element_position, element };
			size_t afterInsertIndex = BinarySearchIndexForInsertElement(pair);

			if (afterInsertIndex == count_of_elements) {
				data[after_last_index] = pair;
				after_last_index = (after_last_index + 1) % size;
			}
			else if (afterInsertIndex == 0) {
				first_index = first_index == 0 ? size - 1 : first_index - 1;
				data[first_index] = pair;
			}
			else {

				size_t startCopyIndex = after_last_index;

				while (startCopyIndex != afterInsertIndex)
				{
					size_t beforeStartCopyIndex = startCopyIndex == 0 ? size - 1 : startCopyIndex - 1;
					
					data[startCopyIndex] = data[beforeStartCopyIndex];

					startCopyIndex = beforeStartCopyIndex;
				}
				data[afterInsertIndex] = pair;

				after_last_index = (after_last_index + 1) % size;
			}

		}
		else {
			data[after_last_index] = element;
			after_last_index = (after_last_index + 1) % size;
		}

		count_of_elements++;
	}
	//else queue overflow -> do nothing
}

template<class T, Order ORDER>
inline bool Queue<T, ORDER>::IsFull() const
{
	return count_of_elements == size;
}

template<class T, Order ORDER>
inline T Queue<T, ORDER>::Pop()
{
	if (!IsEmpty()) {
		const auto& element = data[first_index];

		first_index = (first_index + 1) % size;
		
		count_of_elements--;

		if constexpr (ORDER == Order::ORDERED)
			return element.second;
		else 
			return element;
	}
	//else empty queue -> do nothing
}

template<class T, Order ORDER>
inline bool Queue<T, ORDER>::IsEmpty() const
{
	return count_of_elements == 0;
}

template<class T, Order ORDER>
bool Queue<T, ORDER>::Compare(const std::pair<size_t, T>& right, const std::pair<size_t, T>& left)
{	
	auto& n1 = right.first;
	auto& e1 = right.second;
	
	auto& n2 = left.first;
	auto& e2 = left.second;
	
	if (e1 < e2) {
		return false;
	}
	else if (e2 < e1) {
		return true;
	}

	return n1 < n2;
}

template<class T, Order ORDER>
inline size_t Queue<T, ORDER>::BinarySearchIndexForInsertElement(const std::pair<size_t, T>& element)
{
	
	int startIndex = 0;
	int endIndex = count_of_elements - 1;

	do {
		size_t midIndex = (startIndex + endIndex) / 2;

		size_t midIndexCyclic = (midIndex + first_index) % size;

		//buffer contains only unique elements !! - pair for different places in queue, as idea

		if (Compare(element, data[midIndexCyclic])) {
			endIndex = midIndex - 1;
		}
		else {
			startIndex = midIndex + 1;
		}

	}

	while (endIndex >= startIndex);

	return (startIndex + first_index) % size; //always >= 0

}

template<class T, Order ORDER>
std::ostream& operator<<(std::ostream& stream, const Queue<T, ORDER>& queue)
{
	stream << "[";

	size_t count = 0;

	while (count < queue.count_of_elements) {

		const auto& element = queue.data[(queue.first_index + count) % queue.size];
		if constexpr (ORDER == Order::ORDERED) {
			stream << element.second;
		}
		else {
			stream << element;
		}
		
		if (count < queue.count_of_elements - 1) {
			stream << ", ";
		}

		count++;
	}

	stream << "]";

	return stream;
}
