#ifndef POINT_HPP
#define POINT_HPP

/** @brief Stores an integer x/y coordinate. */
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

	/** @brief Returns the coordinate-wise sum of this point and the operand. */
	Point	operator+(Point &p2)
	{
		return Point(this->x + p2.x, this->y + p2.y);
	}

	/** @brief Returns the coordinate-wise difference between this point and the operand. */
	Point	operator-(Point &p2)
	{
		return Point(this->x - p2.x, this->y - p2.y);
	}
};

#endif