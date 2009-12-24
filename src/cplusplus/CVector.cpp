/*
Copyright (C) 2006 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../headers.h"

Vector::Vector()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector::~Vector()
{
}

bool Vector::operator ==(Vector vector)
{
	return ((x == vector.x) && (y == vector.y) && (z == vector.z));
}

bool Vector::operator !=(Vector vector)
{
	return ((x != vector.x) || (y != vector.y) || (z != vector.z));
}

void Vector::operator =(Vector vector)
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
}

Vector Vector::operator *(float f)
{
	Vector v;
	
	v.set(x * f, y * f, z * f);
	
	return v;
}

void Vector::set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector::operator +=(Vector vector)
{
	this->x += vector.x;
	this->y += vector.y;
	this->z += vector.z;
}

void Vector::operator -=(Vector vector)
{
	this->x -= vector.x;
	this->y -= vector.y;
	this->z -= vector.z;
}

bool Vector::operator <(Vector vector)
{
	return ((x < vector.x) || (y < vector.y) || (z < vector.z));
}

bool Vector::operator >(Vector vector)
{
	return ((x > vector.x) || (y > vector.y) || (z > vector.z));
}

void Vector::normalize()
{
	float n;
	
	n = ((x * x) + (y * y) + (z * z));
	
	if ((n != 1.0) && (n != 0.0))
	{
		n = invSqrt(n);
		
		x *= n;
		y *= n;
		z *= n;
	}
}

float Vector::getLength()
{
	return sqrt((x * x) + (y * y) + (z * z));
}

float Vector::getDistance(Vector v)
{
	return Math::getDistance(*this, v);
}

const char *Vector::toString()
{
	static String s;
	
	s.setText("%f %f %f", x, y, z);
	
	return s.getText();
}

void Vector::set(const char *in)
{
	float x, y, z;
	
	sscanf(in, "%f %f %f", &x, &y, &z);
	
	// stop the compiler moaning :)
	this->x = x;
	this->y = y;
	this->z = z;
}

float Vector::invSqrt(float x) /* Fast Invert Square Root */
{
   union {
      float f;
      int i;
   } u;
      
   float xhalf = 0.5f * x;

   u.f = x;
   u.i = 0x5f3759df - (u.i >> 1);
   x = u.f;
   x = x * (1.5f - xhalf * x * x);

   return x;
}
