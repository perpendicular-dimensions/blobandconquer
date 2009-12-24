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

void Math::limit(signed char *in, int low, int high)
{
	if (*in < low)
	{
		*in = low;
	}
	
	if (*in > high)
	{
		*in = high;
	}
}

void Math::limit(unsigned char *in, int low, int high)
{
	if (*in < low)
	{
		*in = low;
	}
	
	if (*in > high)
	{
		*in = high;
	}
}

void Math::limit(int *in, int low, int high)
{
	if (*in < low)
	{
		*in = low;
	}
	
	if (*in > high)
	{
		*in = high;
	}
}

void Math::limit(float *in, float low, float high)
{
	if (*in < low)
	{
		*in = low;
	}
	
	if (*in > high)
	{
		*in = high;
	}
}

void Math::limit(GLdouble *in, float low, float high)
{
	if (*in < low)
	{
		*in = low;
	}
	
	if (*in > high)
	{
		*in = high;
	}
}

void Math::wrap(signed char *in, signed char low, signed char high)
{
	if (*in < low)
	{
		*in = high;
	}
	
	if (*in > high)
	{
		*in = low;
	}
}

void Math::wrap(int *in, int low, int high)
{
	if (*in < low)
	{
		*in = high;
	}
	
	if (*in > high)
	{
		*in = low;
	}
}

void Math::wrap(float *in, float low, float high)
{
	if (*in < low)
	{
		*in = high;
	}
	
	if (*in > high)
	{
		*in = low;
	}
}

void Math::wrap(GLdouble *in, float low, float high)
{
	if (*in < low)
	{
		*in = high;
	}
	
	if (*in > high)
	{
		*in = low;
	}
}

int Math::rrand(int min, int max)
{
	int r = min;

	max++;

	if ((max - min) == 0)
	{
		return min;
	}

	r += rand() % (max - min);

	return r;
}

float Math::getDistance(float x1, float y1, float x2, float y2)
{
	float x = (x1 - x2);
	float y = (y1 - y2);
	
	x *= x;
	y *= y;
	
	return sqrt(x + y);
}

float Math::getDistance(Vector v1, Vector v2)
{
	float x = (v1.x - v2.x);
	float y = (v1.y - v2.y);
	float z = (v1.z - v2.z);
	
	x *= x;
	y *= y;
	z *= z;
	
	float rtn = (x + y + z);
	
	return (rtn == 0) ? 0 : sqrt(rtn);
}

int Math::toNextPower(int val, int power)
{
	int result = 1;

	while (result < val) result *= power;
	
	return result;
}
