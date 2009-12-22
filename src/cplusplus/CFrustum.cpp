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

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::multiplyMatrix()
{
	glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	
	frustumMatrix[ 0] = (modelMatrix[ 0] * projectionMatrix[ 0]) + (modelMatrix[ 1] * projectionMatrix[ 4]) + (modelMatrix[ 2] * projectionMatrix[ 8]) + (modelMatrix[ 3] * projectionMatrix[12]);
	frustumMatrix[ 1] = (modelMatrix[ 0] * projectionMatrix[ 1]) + (modelMatrix[ 1] * projectionMatrix[ 5]) + (modelMatrix[ 2] * projectionMatrix[ 9]) + (modelMatrix[ 3] * projectionMatrix[13]);
	frustumMatrix[ 2] = (modelMatrix[ 0] * projectionMatrix[ 2]) + (modelMatrix[ 1] * projectionMatrix[ 6]) + (modelMatrix[ 2] * projectionMatrix[10]) + (modelMatrix[ 3] * projectionMatrix[14]);
	frustumMatrix[ 3] = (modelMatrix[ 0] * projectionMatrix[ 3]) + (modelMatrix[ 1] * projectionMatrix[ 7]) + (modelMatrix[ 2] * projectionMatrix[11]) + (modelMatrix[ 3] * projectionMatrix[15]);

	frustumMatrix[ 4] = (modelMatrix[ 4] * projectionMatrix[ 0]) + (modelMatrix[ 5] * projectionMatrix[ 4]) + (modelMatrix[ 6] * projectionMatrix[ 8]) + (modelMatrix[ 7] * projectionMatrix[12]);
	frustumMatrix[ 5] = (modelMatrix[ 4] * projectionMatrix[ 1]) + (modelMatrix[ 5] * projectionMatrix[ 5]) + (modelMatrix[ 6] * projectionMatrix[ 9]) + (modelMatrix[ 7] * projectionMatrix[13]);
	frustumMatrix[ 6] = (modelMatrix[ 4] * projectionMatrix[ 2]) + (modelMatrix[ 5] * projectionMatrix[ 6]) + (modelMatrix[ 6] * projectionMatrix[10]) + (modelMatrix[ 7] * projectionMatrix[14]);
	frustumMatrix[ 7] = (modelMatrix[ 4] * projectionMatrix[ 3]) + (modelMatrix[ 5] * projectionMatrix[ 7]) + (modelMatrix[ 6] * projectionMatrix[11]) + (modelMatrix[ 7] * projectionMatrix[15]);

	frustumMatrix[ 8] = (modelMatrix[ 8] * projectionMatrix[ 0]) + (modelMatrix[ 9] * projectionMatrix[ 4]) + (modelMatrix[10] * projectionMatrix[ 8]) + (modelMatrix[11] * projectionMatrix[12]);
	frustumMatrix[ 9] = (modelMatrix[ 8] * projectionMatrix[ 1]) + (modelMatrix[ 9] * projectionMatrix[ 5]) + (modelMatrix[10] * projectionMatrix[ 9]) + (modelMatrix[11] * projectionMatrix[13]);
	frustumMatrix[10] = (modelMatrix[ 8] * projectionMatrix[ 2]) + (modelMatrix[ 9] * projectionMatrix[ 6]) + (modelMatrix[10] * projectionMatrix[10]) + (modelMatrix[11] * projectionMatrix[14]);
	frustumMatrix[11] = (modelMatrix[ 8] * projectionMatrix[ 3]) + (modelMatrix[ 9] * projectionMatrix[ 7]) + (modelMatrix[10] * projectionMatrix[11]) + (modelMatrix[11] * projectionMatrix[15]);

	frustumMatrix[12] = (modelMatrix[12] * projectionMatrix[ 0]) + (modelMatrix[13] * projectionMatrix[ 4]) + (modelMatrix[14] * projectionMatrix[ 8]) + (modelMatrix[15] * projectionMatrix[12]);
	frustumMatrix[13] = (modelMatrix[12] * projectionMatrix[ 1]) + (modelMatrix[13] * projectionMatrix[ 5]) + (modelMatrix[14] * projectionMatrix[ 9]) + (modelMatrix[15] * projectionMatrix[13]);
	frustumMatrix[14] = (modelMatrix[12] * projectionMatrix[ 2]) + (modelMatrix[13] * projectionMatrix[ 6]) + (modelMatrix[14] * projectionMatrix[10]) + (modelMatrix[15] * projectionMatrix[14]);
	frustumMatrix[15] = (modelMatrix[12] * projectionMatrix[ 3]) + (modelMatrix[13] * projectionMatrix[ 7]) + (modelMatrix[14] * projectionMatrix[11]) + (modelMatrix[15] * projectionMatrix[15]);
}

void Frustum::calculate()
{
	multiplyMatrix();
	
	Plane *p;
	
	// right
	p = &plane[0];
	p->normal.x = frustumMatrix[3]  - frustumMatrix[0];
	p->normal.y = frustumMatrix[7]  - frustumMatrix[4];
	p->normal.z = frustumMatrix[11] - frustumMatrix[8];
	p->d = frustumMatrix[15] - frustumMatrix[12];
   
	// left
	p = &plane[1];
	p->normal.x = frustumMatrix[3]  + frustumMatrix[0];
	p->normal.y = frustumMatrix[7]  + frustumMatrix[4];
	p->normal.z = frustumMatrix[11] + frustumMatrix[8];
	p->d = frustumMatrix[15] + frustumMatrix[12];
   
	// bottom
	p = &plane[2];
	p->normal.x = frustumMatrix[3]  + frustumMatrix[1];
	p->normal.y = frustumMatrix[7]  + frustumMatrix[5];
	p->normal.z = frustumMatrix[11] + frustumMatrix[9];
	p->d = frustumMatrix[15] + frustumMatrix[13];
   
	// top
	p = &plane[3];
	p->normal.x = frustumMatrix[3]  - frustumMatrix[1];
	p->normal.y = frustumMatrix[7]  - frustumMatrix[5];
	p->normal.z = frustumMatrix[11] - frustumMatrix[9];
	p->d = frustumMatrix[15] - frustumMatrix[13];
   
	// far
	p = &plane[4];
	p->normal.x = frustumMatrix[3]  - frustumMatrix[2];
	p->normal.y = frustumMatrix[7]  - frustumMatrix[6];
	p->normal.z = frustumMatrix[11] - frustumMatrix[10];
	p->d = frustumMatrix[15] - frustumMatrix[14];
   
	//near
	p = &plane[5];
	p->normal.x = frustumMatrix[3]  + frustumMatrix[2];
	p->normal.y = frustumMatrix[7]  + frustumMatrix[6];
	p->normal.z = frustumMatrix[11] + frustumMatrix[10];
	p->d = frustumMatrix[15] + frustumMatrix[14];
   
     // Normalize all plane normals
	for (int i = 0 ; i < 6 ; i++)
	{
		plane[i].normalize();
	}
}

int Frustum::classifyPoint(Plane p, Vector v)
{
	double distance = p.normal.x * v.x + p.normal.y * v.y + p.normal.z * v.z - p.d;

	if (distance > EPSILON)
	{
		return POINT_IN_FRONT_OF_PLANE;
	}

	else if (distance < -EPSILON)
	{
		return POINT_BEHIND_PLANE;
	}

	return POINT_ON_PLANE;
}

int Frustum::boundingBoxInsideFrustrum(int x1, int y1, int z1, int x2, int y2, int z2)
{
	for (int i = 0 ; i < 6 ; i++)
	{
		if (plane[i].normal.x * x1 + plane[i].normal.y * y1 + plane[i].normal.z * z1 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x2 + plane[i].normal.y * y1 + plane[i].normal.z * z1 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x1 + plane[i].normal.y * y2 + plane[i].normal.z * z1 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x2 + plane[i].normal.y * y2 + plane[i].normal.z * z1 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x1 + plane[i].normal.y * y1 + plane[i].normal.z * z2 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x2 + plane[i].normal.y * y1 + plane[i].normal.z * z2 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x1 + plane[i].normal.y * y2 + plane[i].normal.z * z2 + plane[i].d > 0) continue;
		if (plane[i].normal.x * x2 + plane[i].normal.y * y2 + plane[i].normal.z * z2 + plane[i].d > 0) continue;

		return 0;
	}

	return 1;
}

bool Frustum::containsSphere(Vector position, float radius)
{
	Plane *p;
	
	for (int i = 0 ; i < 6 ; i++)
	{
		p = &plane[i];
		
		if ((p->normal.x * position.x) + (p->normal.y * position.y) + (p->normal.z * position.z) + p->d <= -radius)
		{
			return false;
		}
	}
	
	return true;
}

bool Frustum::containsEntity(Entity *entity)
{
	return containsSphere(entity->position, entity->boundingBox.getSize());
}
