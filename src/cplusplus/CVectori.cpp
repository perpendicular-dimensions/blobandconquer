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

Vectori::Vectori()
{
	x = 0;
	y = 0;
	z = 0;
}

Vectori::~Vectori()
{
}

bool Vectori::operator ==(Vectori vectori)
{
	return ((x == vectori.x) && (y == vectori.y) && (z == vectori.z));
}

bool Vectori::operator !=(Vectori vectori)
{
	return (!((x == vectori.x) && (y == vectori.y) && (z == vectori.z)));
}

void Vectori::operator =(Vectori vectori)
{
	this->x = vectori.x;
	this->y = vectori.y;
	this->z = vectori.z;
}

Vectori Vectori::operator *(int f)
{
	Vectori v;
	
	v.set(x * f, y * f, z * f);
	
	return v;
}

void Vectori::set(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vectori::operator +=(Vectori vectori)
{
	this->x += vectori.x;
	this->y += vectori.y;
	this->z += vectori.z;
}

void Vectori::operator -=(Vectori vectori)
{
	this->x -= vectori.x;
	this->y -= vectori.y;
	this->z -= vectori.z;
}

void Vectori::normalize()
{
	float n;
	
	n = ((x * x) + (y * y) + (z * z));
	
	if ((n != 1.0) && (n != 0.0))
	{
		n = invSqrt(n);
		
		x = (int)(x * n);
		y = (int)(y * n);
		z = (int)(z * n);
	}
}

const char *Vectori::toString()
{
	static String s;
	
	s.setText("%d %d %d", x, y, z);
	
	return s.getText();
}

void Vectori::set(const char *in)
{
	int x, y, z;
	
	sscanf(in, "%d %d %d", &x, &y, &z);
	
	this->x = x;
	this->y = y;
	this->z = z;
}

float Vectori::invSqrt(float x) /* Fast Invert Square Root */
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
