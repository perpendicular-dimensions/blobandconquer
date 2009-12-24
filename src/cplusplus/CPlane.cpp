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

Plane::Plane()
{
	d = 0;
}

Plane::~Plane()
{
}

void Plane::normalize()
{
	float n;
	
	n = ((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
	
	if (n != 1.0)
	{
		n = invSqrt(n);
		
		normal.x *= n;
		normal.y *= n;
		normal.z *= n;
		
		d *= n;
	}
}

float Plane::invSqrt(float x) /* Fast Invert Square Root */
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
