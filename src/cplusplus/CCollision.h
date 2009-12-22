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

class Collision {

	private:

		Collision(){}

	public:

	static bool collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
	static bool collision(float x1, float y1, float z1, float w1, float h1, float l1, float x2, float y2, float z2, float w2, float h2, float l2);
	static bool collision(Vector position1, BoundingBox box1, Vector position2, BoundingBox box2);
	static bool collision(Entity *e1, Entity *e2);

};
