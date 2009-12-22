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

class Geometry {

	private:

	Geometry();
		
	public:
		
	static Vector getNormal(Face f);
	static Vector getVectorFromPoint(Vector v1, Vector v2);
	static Vector getCrossProduct(Vector v1, Vector v2);
	static double rayIntersection(Vector origin, Vector dir, Vector planeNormal, double planeD);
	static int lineIntersectPlane(Vector lineStart, Vector lineEnd, Vector normal, double planeD);
	static int pointInTriangle(Vector point, Vector p1, Vector p2, Vector p3);
	static Vector multiplyVectorByScaler(Vector v1, double scaler);
	static double getPlaneDistance(Vector normal, Vector point);
	static Vector responseBounce(Vector dir, Vector normal, float elasticity);
	static Vector responseSlide(Vector dir, Vector normal);
};
