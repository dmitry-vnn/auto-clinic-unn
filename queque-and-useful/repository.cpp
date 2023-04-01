#include "repository.h"

std::unique_ptr<const Patient> Repository::Pop()
{
	if (_queue.IsEmpty())
	{
		throw std::out_of_range("Repository is empty");
	}

	return _queue.Pop();
}

void Repository::Push(std::unique_ptr<const Patient> patient)
{
	if (_queue.IsFull())
	{
		throw std::out_of_range("Repository is full");
	}

	_queue.Push(std::move(patient));
}

const Doctor* Repository::Authorize(const DoctorAuthData& authData) const
{
	for (const auto& doc : _doctors)
	{
		if (doc.CanAuth(authData))
		{
			return &doc;
		}
	}

	return nullptr;
}
