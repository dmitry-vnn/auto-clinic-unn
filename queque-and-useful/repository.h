#pragma once
#include "doctor.h"
#include "queue.h"

class Repository
{

private:
	Queue<std::unique_ptr<const Patient>, Order::ORDERED> _queue;
	std::vector<Doctor> _doctors;

public:

	explicit Repository(std::vector<Doctor> doctors) : _queue(100), _doctors(std::move(doctors)) {}

	Repository(const Repository& other) = delete;
	Repository& operator=(const Repository& other) = delete;

public:
	std::unique_ptr<const Patient> Pop();
	void Push(std::unique_ptr<const Patient> patient);

	size_t Size() const { return _queue.Size(); }
	void Clear() { _queue.Clear();  }

	bool IsFull() const { return _queue.IsFull();  }
	bool IsEmpty() const { return _queue.IsEmpty();  }

	const Doctor* Authorize(const DoctorAuthData& authData) const;



};
