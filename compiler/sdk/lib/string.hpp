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