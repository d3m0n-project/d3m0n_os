#ifndef STRING_HPP
#define STRING_HPP

#include "types.h"

class string
{
private:
	char	*_data;
	size_t	_length;

public:
	string(): _data(nullptr), _length(0)
	{}

	string(const char *str): _data(nullptr), _length(0)
	{
		if (!str)
			return;

		while (str[_length])
			++_length;

		_data = new char[_length + 1];

		for (size_t i = 0; i < _length; ++i)
			_data[i] = str[i];

		_data[_length] = '\0';
	}

	string(const string &other): _data(nullptr), _length(other._length)
	{
		if (!other._data)
			return;

		_data = new char[_length + 1];

		for (size_t i = 0; i < _length; ++i)
			_data[i] = other._data[i];

		_data[_length] = '\0';
	}

	string &operator=(const string &other)
	{
		char *copy;

		if (this == &other)
			return *this;

		copy = nullptr;

		if (other._data)
		{
			copy = new char[other._length + 1];

			for (size_t i = 0; i < other._length; ++i)
				copy[i] = other._data[i];

			copy[other._length] = '\0';
		}

		delete[] _data;
		_data = copy;
		_length = other._length;

		return *this;
	}

	string operator+(const string &other) const
	{
		string result;
		size_t i;

		result._length = _length + other._length;
		result._data = new char[result._length + 1];

		for (i = 0; i < _length; ++i)
			result._data[i] = _data[i];

		for (i = 0; i < other._length; ++i)
			result._data[_length + i] = other._data[i];

		result._data[result._length] = '\0';

		return result;
	}

	~string()
	{
		delete[] _data;
	}

	size_t length() const
	{
		return _length;
	}

	const char *c_str() const
	{
		return _data ? _data : "";
	}

	char &operator[](size_t index)
	{
		return _data[index];
	}

	const char &operator[](size_t index) const
	{
		return _data[index];
	}
};


#endif
