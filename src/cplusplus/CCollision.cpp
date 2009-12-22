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

bool Collision::collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
	float x3 = x1 + w1;
	float y3 = y1 + h1;

	float x4 = x2 + w2;
	float y4 = y2 + h2;

	return !(x3 < x2 || x4 < x1 || y3 < y2 || y4 < y1);
}

bool Collision::collision(float x1, float y1, float z1, float w1, float h1, float l1, float x2, float y2, float z2, float w2, float h2, float l2)
{
	float x3 = x1 + w1;
	float y3 = y1 + h1;
	float z3 = z1 + l1;

	float x4 = x2 + w2;
	float y4 = y2 + h2;
	float z4 = z2 + l2;

	return !(x3 < x2 || x4 < x1 || y3 < y2 || y4 < y1 || z3 < z2 || z4 < z1);
}

bool Collision::collision(Vector p1, BoundingBox bb1, Vector p2, BoundingBox bb2)
{
	Vector size1, size2;
	
	p1 += bb1.mins;
	
	p2 += bb2.mins;
	
	size1 = bb1.maxs;
	size1.x += fabs(bb1.mins.x);
	size1.y += fabs(bb1.mins.y);
	size1.z += fabs(bb1.mins.z);
	
	size2 = bb2.maxs;
	size2.x += fabs(bb2.mins.x);
	size2.y += fabs(bb2.mins.y);
	size2.z += fabs(bb2.mins.z);
	
	return collision(p1.x, p1.y, p1.z, size1.x, size1.y, size1.z,
					 p2.x, p2.y, p2.z, size2.x, size2.y, size2.z);
}

bool Collision::collision(Entity *e1, Entity *e2)
{
	return collision(e1->position, e1->boundingBox, e2->position, e2->boundingBox);
}
