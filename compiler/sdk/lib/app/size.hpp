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
	bool		percentage;
public:
	Size(void)
	{
		this->relative = 0;
		this->size = 0;
		this->percentage = false;
	}

	Size(uint32_t s)
	{
		this->relative = 0;
		this->size = s;
		this->percentage = false;
	}

	Size(uint32_t percentage, uint32_t *relative)
	{
		//if (!relative)
		//	throw new SizeException();
		this->size = percentage;
		this->relative = relative;
		this->percentage = false;
	}

	Size(uint32_t percentage, bool relative_size)
	{
		this->size = percentage;
		this->relative = 0;
		this->percentage = relative_size;
	}

	uint32_t	get(void) const
	{
		if (this->relative)
			return (this->size * (*this->relative));
		else
			return this->size;
	}

	uint32_t	get(uint32_t parent_size) const
	{
		if (this->percentage)
			return (this->size * parent_size) / 100;
		if (this->relative)
			return (this->size * parent_size) / 100;
		return this->size;
	}
};

#endif
