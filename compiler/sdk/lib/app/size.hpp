#ifndef SIZE_HPP
#define SIZE_HPP

#include "string.hpp"
#include <exception>

using namespace std;

//class SizeException : public exception {
//public:
//	SizeException() {}

//	const char* what() const noexcept override {
//		return "Invalid size gived!";
//	}
//};

class Size
{
private:
	uint32_t	size;
	uint32_t	*relative;
public:
	Size(void)
	{
		this->relative = 0;
		this->size = 0;
	}

	Size(uint32_t s)
	{
		this->relative = 0;
		this->size = s;
	}

	Size(uint32_t percentage, uint32_t *relative)
	{
		//if (!relative)
		//	throw new SizeException();
		this->size = percentage;
		this->relative = relative;
	}

	uint32_t	get(void)
	{
		if (this->relative)
			return (this->size * (*this->relative));
		else
			return this->size;
	}
};

#endif