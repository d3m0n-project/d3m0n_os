#ifndef POINT_HPP
#define POINT_HPP

/** @brief Represents the Point type. */
class	Point
{
public:
	int		x;
	int		y;

	Point(void)
	{
		this->x = 0;
		this->y = 0;
	}

	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	/** @brief Implements the + operation. */
	Point	operator+(Point &p2)
	{
		return Point(this->x + p2.x, this->y + p2.y);
	}

	/** @brief Implements the - operation. */
	Point	operator-(Point &p2)
	{
		return Point(this->x - p2.x, this->y - p2.y);
	}
};

#endif