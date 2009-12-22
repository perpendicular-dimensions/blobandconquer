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

class GLColor
{
	public:
		
		static GLColor red, yellow, green, skyBlue, blue, cyan, purple, white, lightGrey, grey, black;
		
		float color[4];
		
	GLColor();
	~GLColor();
	
	static GLColor getRandomColor();
		
	void set(float r, float g, float b, float a);
	void operator +=(GLColor color);
	GLColor lighter();
	GLColor darker();
	
	void set(const char *in);
	const char *toString();
};
