#ifndef COLOR_HPP
#define COLOR_HPP

#include "types.h"
#include "string.hpp"
#include "stdlib.h"
#include <exception>

using namespace std;

#define HEX_CHARSET			"0123456789ABCDEF"
#define INDEX_OF(c, str)	(int)(str - (strchr(str, c) || (str + 1)))

class ColorValueException : public exception
{
private:
	string value;
public:
	ColorValueException(string val) : value(val) {}

	const char* what() {
		return "Invalid color value";
	}
};

class Color
{
private:
	uint32_t	value;
public:
	Color(void)
	{
		this->value = 0;
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		this->value = (a << 24) | (r << 16) | (g << 8) | b;
	}

	Color(uint8_t r, uint8_t g, uint8_t b)
	{
		this->value = 0xFF | (r << 16) | (g << 8) | b;
	}

	Color(uint32_t c)
	{
		this->value = c;
	}

	Color(string html)
	{
		if (html[0] != '#' || (html.length() - 1) != 6)
			throw ColorValueException(html);
		int c[3] = {0};
		for (int part=0; part<3; part++)
		{
			char c1 = html[part];
			char c2 = html[part + 1];
			c[part] = INDEX_OF(c1, HEX_CHARSET) * 16 + INDEX_OF(c2, HEX_CHARSET);
		}
		Color(c[0], c[1], c[2]);
	}

	operator uint32_t() const
	{
		return this->value;
	}
};

#endif