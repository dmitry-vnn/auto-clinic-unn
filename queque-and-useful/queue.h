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
	
	std::conditional_t<ORDER == Order::ORDERED, std::pair<size_t, T>*, T*> _data;
	size_t _size;

	size_t _firstIndex;
	size_t _afterLastIndex;

	size_t _countOfElements;

	size_t _absoluteLastElementPosition;

public:
	explicit Queue(size_t initialCapacity = 10);
	explicit Queue(std::initializer_list<T> list);
	~Queue();

public:

	void Push(T element);
	bool IsFull() const;

	Queue& operator+=(T element);

	T Pop();
	bool IsEmpty() const;

	size_t Size() const { return _countOfElements; }

	void Clear() { _firstIndex = _afterLastIndex = _countOfElements = _absoluteLastElementPosition = 0; }

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
Queue<T, ORDER>::Queue(const size_t initialCapacity) :
	_size(initialCapacity),
	_firstIndex(0),
	_afterLastIndex(0),
	_countOfElements(0),
	_absoluteLastElementPosition(0)
{
	if constexpr (ORDER == Order::ORDERED)
		_data = new std::pair<size_t, T>[_size];
	else
		_data = new T[_size];
}

template<class T, Order ORDER>
Queue<T, ORDER>::Queue(std::initializer_list<T> list) :
	_size(list.size()),
	_firstIndex(0),
	_afterLastIndex(0),
	_countOfElements(list.size()),
	_absoluteLastElementPosition(0)
{
	if constexpr (ORDER == Order::ORDERED) {
		_data = new std::pair<size_t, T>[_size];
		
		for (size_t i = 0; i < _size; i++)
		{
			_data[i] = std::make_pair( i, std::move(list.begin()[i]) );
		}

		std::sort(_data, _data + _size, Compare);
	}
	else {
		_data = new T[_size];
		std::copy(list.begin(), list.end(), _data);
	}

}

template<class T, Order ORDER>
Queue<T, ORDER>::~Queue()
{
	delete[] _data;
	_data = nullptr;
}

template<class T, Order ORDER>
void Queue<T, ORDER>::Push(T element)
{
	if (!IsFull()) {

		if constexpr (ORDER == Order::ORDERED) {
			std::pair<size_t, T> pair = std::make_pair(++_absoluteLastElementPosition, std::move(element));
			size_t afterInsertIndex = IsEmpty() ? _afterLastIndex : BinarySearchIndexForInsertElement(pair);
			
			if (afterInsertIndex == _afterLastIndex) {
				_data[_afterLastIndex] = std::move(pair);
				_afterLastIndex = (_afterLastIndex + 1) % _size;
			}
			else if (afterInsertIndex == _firstIndex) {
				_firstIndex = _firstIndex == 0 ? _size - 1 : _firstIndex - 1;
				_data[_firstIndex] = std::move(pair);
			}
			else {
			
				size_t startCopyIndex = _afterLastIndex;
			
				while (startCopyIndex != afterInsertIndex)
				{
					size_t beforeStartCopyIndex = startCopyIndex == 0 ? _size - 1 : startCopyIndex - 1;
					
					_data[startCopyIndex] = std::move(_data[beforeStartCopyIndex]);
			
					startCopyIndex = beforeStartCopyIndex;
				}
				_data[afterInsertIndex] = std::move(pair);
			
				_afterLastIndex = (_afterLastIndex + 1) % _size;
			}

		}
		else {
			_data[_afterLastIndex] = std::move(element);
			_afterLastIndex = (_afterLastIndex + 1) % _size;
		}

		_countOfElements++;
	}
	//else queue overflow -> do nothing
}

template<class T, Order ORDER>
bool Queue<T, ORDER>::IsFull() const
{
	return _countOfElements == _size;
}

template <class T, Order ORDER>
Queue<T, ORDER>& Queue<T, ORDER>::operator+=(T element)
{
	Push(std::move(element));
	return *this;
}

template<class T, Order ORDER>
T Queue<T, ORDER>::Pop()
{
	if (!IsEmpty()) {
		
		_countOfElements--;

		if constexpr (ORDER == Order::ORDERED)
		{
			auto patient = std::move(_data[_firstIndex].second);
			_firstIndex = (_firstIndex + 1) % _size;
			return std::move(patient);
		}
		else
			return _data[_firstIndex];
	}

	throw std::logic_error("Queue is empty");
}

template<class T, Order ORDER>
bool Queue<T, ORDER>::IsEmpty() const
{
	return _countOfElements == 0;
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
	
	if (e2 < e1) {
		return true;
	}
	
	return n1 < n2;
}

template<class T, Order ORDER>
size_t Queue<T, ORDER>::BinarySearchIndexForInsertElement(const std::pair<size_t, T>& element)
{
	
	int startIndex = 0;
	int endIndex = _countOfElements - 1;
	
	do {
		size_t midIndex = (startIndex + endIndex) / 2;
	
		size_t midIndexCyclic = (midIndex + _firstIndex) % _size;
	
		//buffer contains only unique elements !! - pair for different places in queue, as idea
	
		if (Compare(element, _data[midIndexCyclic])) {
			endIndex = midIndex - 1;
		}
		else {
			startIndex = midIndex + 1;
		}
	
	}
	
	while (endIndex >= startIndex);
	
	return (startIndex + _firstIndex) % _size; //always >= 0

}

template<class T, Order ORDER>
std::ostream& operator<<(std::ostream& stream, const Queue<T, ORDER>& queue)
{
	stream << "[";
	
	size_t count = 0;
	
	while (count < queue._countOfElements) {
	
		const auto& element = queue._data[(queue._firstIndex + count) % queue._size];
		if constexpr (ORDER == Order::ORDERED) {
			stream << element.second;
		}
		else {
			stream << element;
		}
		
		if (count < queue._countOfElements - 1) {
			stream << ", ";
		}
	
		count++;
	}
	
	stream << "]";

	return stream;
}
