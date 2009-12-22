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

BoundingBox::BoundingBox()
{
	size = -1;
	
	horizontalSize = -1;
	
	drawSize = -1;
	
	plane[LEFT].normal.x   = -1;
	plane[LEFT].normal.y   =  0;
	plane[LEFT].normal.z   =  0;

	plane[RIGHT].normal.x  =  1;
	plane[RIGHT].normal.y  =  0;
	plane[RIGHT].normal.z  =  0;

	plane[TOP].normal.x    =  0;
	plane[TOP].normal.y    =  1;
	plane[TOP].normal.z    =  0;

	plane[BOTTOM].normal.x =  0;
	plane[BOTTOM].normal.y = -1;
	plane[BOTTOM].normal.z =  0;

	plane[FRONT].normal.x  =  0;
	plane[FRONT].normal.y  =  0;
	plane[FRONT].normal.z  = -1;

	plane[BACK].normal.x   =  0;
	plane[BACK].normal.y   =  0;
	plane[BACK].normal.z   =  1;

	boxType = BOX_SOLID;
}

BoundingBox::~BoundingBox()
{
}

int BoundingBox::getSize()
{
	if (size == -1)
	{
		size = max((int)mins.x, (int)maxs.x);
		
		size = max((int)size, (int)mins.y);
		size = max((int)size, (int)maxs.y);
		
		size = max((int)size, (int)mins.z);
		size = max((int)size, (int)maxs.z);
	}
	
	if (size <= 0)
	{
		printf("ERROR: BoundingBox::getSize() - Size is %d. Must be > 0\n", size);
		//abort();
	}
	
	return size;
}

int BoundingBox::getDrawSize()
{
	if (drawSize == -1)
	{
		drawSize = getSize() * 250;
	}
	
	return drawSize;
}

int BoundingBox::getHorizontalSize()
{
	if (horizontalSize == -1)
	{
		horizontalSize = max((int)mins.x, (int)maxs.x);
		
		horizontalSize = max((int)size, (int)mins.y);
		horizontalSize = max((int)size, (int)maxs.y);
	}
	
	if (horizontalSize <= 0)
	{
		printf("ERROR: BoundingBox::getHorizontalSize() - Size is %d. Must be > 0\n", size);
		//abort();
	}
	
	return horizontalSize;
}

void BoundingBox::update(Vector position)
{
	Vector point1;

	/* Calculate Distance for Left Face */

	point1.x = position.x + mins.x;
	point1.y = position.y + maxs.y;
	point1.z = position.z + maxs.z;

	plane[LEFT].d = Geometry::getPlaneDistance(plane[LEFT].normal, point1);

	/* Calculate Distance for Right Face */

	point1.x = position.x + maxs.x;
	point1.y = position.y + maxs.y;
	point1.z = position.z + mins.z;

	plane[RIGHT].d = Geometry::getPlaneDistance(plane[RIGHT].normal, point1);

	/* Calculate Distance for Top Face */

	point1.x = position.x + mins.x;
	point1.y = position.y + maxs.y;
	point1.z = position.z + maxs.z;

	plane[TOP].d = Geometry::getPlaneDistance(plane[TOP].normal, point1);

	/* Calculate Distance for Bottom Face */

	point1.x = position.x + mins.x;
	point1.y = position.y + mins.y;
	point1.z = position.z + maxs.z;

	plane[BOTTOM].d = Geometry::getPlaneDistance(plane[BOTTOM].normal, point1);

	/* Calculate Distance for Front Face */

	point1.x = position.x + mins.x;
	point1.y = position.y + maxs.y;
	point1.z = position.z + mins.z;

	plane[FRONT].d = Geometry::getPlaneDistance(plane[FRONT].normal, point1);

	/* Calculate Distance for Back Face */

	point1.x = position.x + maxs.x;
	point1.y = position.y + maxs.y;
	point1.z = position.z + maxs.z;

	plane[BACK].d = Geometry::getPlaneDistance(plane[BACK].normal, point1);
}
