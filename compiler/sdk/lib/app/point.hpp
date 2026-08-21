#ifndef POINT_HPP
#define POINT_HPP

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

	Point	operator+(Point &p2)
	{
		return Point(this->x + p2.x, this->y + p2.y);
	}

	Point	operator-(Point &p2)
	{
		return Point(this->x - p2.x, this->y - p2.y);
	}
};

#endif