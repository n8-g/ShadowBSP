
#include "Polygon.h"

///////////////////////////////////////////////////////////////////////////////

Point::Point(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Point::Point(const Point &copy)
{
	this->x = copy.x;
	this->y = copy.y;
	this->z = copy.z;
}

///////////////////////////////////////////////////////////////////////////////

Polygon::Polygon(const Point &a, const Point &b, const Point &c)
: _a(a), _b(b), _c(c)
{
	// every polygon is lit by default
	_is_lit = true;
}

Polygon::Polygon(const Polygon &copy)
: _a(copy[0]), _b(copy[1]), _c(copy[2])
{
	_is_lit = copy._is_lit;
}

///////////////////////////////////////////////////////////////////////////////

Point& Polygon::operator[](int index)
{
	switch (index) {
		case 0:
			return _a;
		case 1:
			return _b;
		case 2:
			return _c;
	}

	// always returns the first vertex if the index is invalid
	return _a;
}

const Point& Polygon::operator[](int index) const
{
	switch (index) {
		case 0:
			return _a;
		case 1:
			return _b;
		case 2:
			return _c;
	}

	// always returns the first vertex if the index is invalid
	return _a;
}
