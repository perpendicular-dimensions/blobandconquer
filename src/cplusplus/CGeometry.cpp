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

Vector Geometry::getNormal(Face f)
{
	Vector v = getCrossProduct(getVectorFromPoint(f.point[2], f.point[0]), getVectorFromPoint(f.point[1], f.point[0]));
	
	v.normalize();
	
	return v;
}

Vector Geometry::getVectorFromPoint(Vector v1, Vector v2)
{
	v1.x = v1.x - v2.x;
	v1.y = v1.y - v2.y;
	v1.z = v1.z - v2.z;

	return v1;
}

Vector Geometry::getCrossProduct(Vector v1, Vector v2)
{
	Vector v;

	v.x = (v1.y * v2.z) - (v1.z * v2.y);
	v.y = (v1.z * v2.x) - (v1.x * v2.z);
	v.z = (v1.x * v2.y) - (v1.y * v2.x);

	return v;
}

double Geometry::rayIntersection(Vector origin, Vector dir, Vector planeNormal, double planeD)
{
	float cosAlpha, deltaD;

	cosAlpha = dotProd(dir, planeNormal);

	if (cosAlpha == 0)
	{
		return 1.0f;
	}

	deltaD = -planeD - dotProd(origin, planeNormal);

	return (deltaD / cosAlpha);
}

int Geometry::lineIntersectPlane(Vector lineStart, Vector lineEnd, Vector normal, double planeD)
{
	float distance1, distance2;

	distance1 = dotProd(normal, lineStart) + planeD;
	distance2 = dotProd(normal, lineEnd)   + planeD;

	return (distance2 == 0 ? 1 : distance1 * distance2 < 0);
}

int Geometry::pointInTriangle(Vector point, Vector p1, Vector p2, Vector p3)
{
	double angle;
	Vector v1, v2, v3;
	
	v1 = point;
	v2 = point;
	v3 = point;
	
	v1 -= p1;
	v1 -= p2;
	v1 -= p2;
	
	v1.normalize();
	v2.normalize();
	v3.normalize();

	angle  = acos(dotProd(v1, v2));
	angle += acos(dotProd(v2, v3));
	angle += acos(dotProd(v3, v1));

	return (fabs(angle - 2 * PI) < EPSILON);
}

Vector Geometry::multiplyVectorByScaler(Vector v1, double scaler)
{
	v1.x *= scaler;
	v1.y *= scaler;
	v1.z *= scaler;

	return v1;
}

double Geometry::getPlaneDistance(Vector normal, Vector point)
{
	return (dotProd(normal, point) * -1);
}

/* Returns the reflection direction */

Vector Geometry::responseBounce(Vector dir, Vector normal, float elasticity)
{
	Vector newDir;

	newDir = multiplyVectorByScaler(normal, dotProd(dir, normal) * elasticity);
	dir -= newDir;

	return dir;
}

/* Returns sliding direction */

Vector Geometry::responseSlide(Vector dir, Vector normal)
{
	Vector newDir;

	newDir = multiplyVectorByScaler(normal, dotProd(dir, normal));
	dir -= newDir;

	return dir;
}
