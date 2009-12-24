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

GLColor::GLColor()
{
	color[0] = color[1] = color[2] = color[3] = 1;
	
	red.set(1.0, 0.0, 0.0, 1.0);
	yellow.set(1.0, 1.0, 0.0, 1.0);
	green.set(0.0, 1.0, 0.0, 1.0);
	skyBlue.set(0.5, 0.5, 1.0, 1.0);
	blue.set(0.0, 0.0, 1.0, 1.0);
	cyan.set(0.0, 0.5, 1.0, 1.0);
	white.set(1.0, 1.0, 1.0, 1.0);
	lightGrey.set(0.7, 0.7, 0.7, 1.0);
	grey.set(0.5, 0.5, 0.5, 1.0);
	black.set(0.0, 0.0, 0.0, 1.0);
	purple.set(1.0, 0.0, 1.0, 1.0);
}

GLColor::~GLColor()
{
}

void GLColor::operator +=(GLColor c)
{
	for (int i = 0 ; i < 4 ; i++)
	{
		color[i] += c.color[i];
		Math::limit(&color[i], 0, 1);
	}
}

void GLColor::set(float r, float g, float b, float a)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

GLColor GLColor::getRandomColor()
{
	switch (rand() % 10)
	{
		case 0:
			return red;
			break;
		case 1:
			return yellow;
			break;
		case 2:
			return green;
			break;
		case 3:
			return skyBlue;
			break;
		case 4:
			return blue;
			break;
		case 5:
			return cyan;
			break;
		case 6:
			return white;
			break;
		case 7:
			return lightGrey;
			break;
		case 8:
			return grey;
			break;
		case 9:
			return purple;
			break;
	}
	
	return black;
}

GLColor GLColor::lighter()
{
	GLColor c = *this;
	
	c.color[0] *= 1.5;
	c.color[1] *= 1.5;
	c.color[2] *= 1.5;
	
	return c;
}

GLColor GLColor::darker()
{
	GLColor c = *this;
	
	c.color[0] *= 0.5;
	c.color[1] *= 0.5;
	c.color[2] *= 0.5;
	
	return c;
}

void GLColor::set(const char *in)
{
	sscanf(in, "%f %f %f %f", &color[0], &color[1], &color[2], &color[3]);
}

const char *GLColor::toString()
{
	static String s;
	
	s.setText("%.2f %.2f %.2f %.2f", color[0], color[1], color[2], color[3]);
	
	return s.getText();
}

GLColor GLColor::red;
GLColor GLColor::yellow;
GLColor GLColor::green;
GLColor GLColor::skyBlue;
GLColor GLColor::blue;
GLColor GLColor::cyan;
GLColor GLColor::white;
GLColor GLColor::lightGrey;
GLColor GLColor::grey;
GLColor GLColor::black;
GLColor GLColor::purple;
