#include "../headers.h"

Point::Point()
{
	x = y = 0;
}

Point::~Point()
{
}

bool Point::operator ==(Point point)
{
	return ((x == point.x) && (y == point.y));
}

bool Point::operator !=(Point point)
{
	return (!((x == point.x) && (y == point.y)));
}

void Point::operator =(Point point)
{
	this->x = point.x;
	this->y = point.y;
}

Point Point::operator *(float f)
{
	Point v;
	
	v.set(x * f, y * f);
	
	return v;
}

void Point::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Point::operator +=(Point point)
{
	this->x += point.x;
	this->y += point.y;
}

void Point::operator -=(Point point)
{
	this->x -= point.x;
	this->y -= point.y;
}

float Point::getDistance(Point *point)
{
	return Math::getDistance(x, y, point->x, point->y);
}
