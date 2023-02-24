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
	T* data;
	size_t size;

	size_t first_index;
	size_t after_last_index;

	size_t count_of_elements;

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

	static bool compare(const T& right, const T& left);

};

template<class T, Order ORDER>
inline Queue<T, ORDER>::Queue(size_t initial_capacity) :
	size(initial_capacity),
	data(new T[initial_capacity]),
	first_index(0),
	after_last_index(0),
	count_of_elements(0)
{
}

template<class T, Order ORDER>
inline Queue<T, ORDER>::Queue(std::initializer_list<T> list) :
	size(list.size()),
	first_index(0),
	after_last_index(0),
	count_of_elements(list.size())
{
	data = new T[size];
	std::copy(list.begin(), list.end(), data);

	if (ORDER == Order::ORDERED) {
		std::sort(data, data + size, compare);
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
	if (count_of_elements < size) {
		data[after_last_index] = element;

		after_last_index = (after_last_index + 1) % size;

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
	if (count_of_elements > 0) {
		const T& element = data[first_index];

		first_index = (first_index + 1) % size;
		
		count_of_elements--;

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
bool Queue<T, ORDER>::compare(const T& right, const T& left)
{
	return right < left;
}

template<class T, Order ORDER>
std::ostream& operator<<(std::ostream& stream, const Queue<T, ORDER>& queue)
{
	stream << "[";

	size_t count = 0;

	while (count < queue.count_of_elements) {
		stream << queue.data[(queue.first_index + count) % queue.size];
		
		if (count < queue.count_of_elements - 1) {
			stream << ", ";
		}

		count++;
	}

	stream << "]";

	return stream;
}
