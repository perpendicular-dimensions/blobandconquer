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

EntityDef::EntityDef()
{
	type = -1;
	
	bbMins.set(-3, -3, -3);
	bbMaxs.set(3, 3, 3);
	
	textureName[0] = "NULL";
	textureName[1] = "NULL";
	modelName[0] = "NULL";
	modelName[1] = "NULL";
	
	texture[0] = NULL;
	texture[1] = NULL;
	model[0] = NULL;
	model[1] = NULL;
	md2 = NULL;
	
	flags = 0;
}

EntityDef::~EntityDef()
{
}

const char *EntityDef::getName()
{
	return name.getText();
}
