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

// abstract class

class EntityDef : public Linkable {

	public:
		
		int type;
		
		Vector bbMins, bbMaxs;
		
		String name;
		
		String modelName[2];
		String textureName[2];
		
		Texture *texture[2];
		SimpleModel *model[2];
		MD2Model *md2;
		
		long flags;
		
	EntityDef();
	virtual ~EntityDef() = 0;
	
	const char *getName();

};
