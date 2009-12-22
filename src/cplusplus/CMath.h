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

class Math {

	private:

		Math(){}

	public:

	static void limit(signed char *in, int low, int high);
	static void limit(unsigned char *in, int low, int high);
	static void limit(int *in, int low, int high);
	static void limit(float *in, float low, float high);
	static void limit(GLdouble *in, float low, float high);
	
	static void wrap(signed char *in, signed char low, signed char high);
	static void wrap(int *in, int low, int high);
	static void wrap(float *in, float low, float high);
	static void wrap(GLdouble *in, float low, float high);
	
	static int rrand(int min, int max);
	
	static float getDistance(float x1, float y1, float x2, float y2);
	static float getDistance(Vector v1, Vector v2);
	
	static int toNextPower(int val, int power);
};
