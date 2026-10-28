#ifndef COLOR_HPP
#define COLOR_HPP

#include "types.h"
#include "string.hpp"
#include "stdlib.h"
#include <exception>

using namespace std;

#define HEX_CHARSET			"0123456789ABCDEF"
#define INDEX_OF(c, str)	(uint8_t)((const char *)(strchr(str, c)?strchr(str, c):str) - str)

/** @brief Reports an invalid color value. */
class ColorValueException : public exception
{
private:
	string value;
public:
	ColorValueException(string val) : value(val) {}

	/** @brief Returns the exception message. */
	const char* what() {
		return "Invalid color value";
	}
};

/** @brief Stores a color value and converts between packed and textual color forms. */
class Color
{
private:
	uint32_t	value = 0x00000000;
public:
	Color(void)
	{
		this->value = 0x00000000;
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		this->value = (a << 24) | (b << 16) | (g << 8) | r;
	}

	Color(uint8_t r, uint8_t g, uint8_t b)
	{
		this->value = 0xFF000000 | (b << 16) | (g << 8) | r;
	}

	Color(uint32_t c)
	{
		this->value = c;
	}

	Color(string html)
	{
		if (html[0] != '#' || (html.length() - 1) != 6)
			throw ColorValueException(html);
		uint8_t c[3] = {0};
		for (int part=0; part<3; part++)
		{
			char c1 = toupper(html[1 + (part * 2)]);
			char c2 = toupper(html[2 + (part * 2)]);
			c[part] = INDEX_OF(c1, HEX_CHARSET) * 16 + INDEX_OF(c2, HEX_CHARSET);
		}
		this->value = 0xFF000000 | (c[0] << 16) | (c[1] << 8) | c[2];
	}

	/** @brief Returns the color packed into a 32-bit RGBA value. */
	operator uint32_t() const
	{
		return this->value;
	}
};

#endif