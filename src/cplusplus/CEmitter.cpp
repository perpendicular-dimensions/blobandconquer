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

Emitter::Emitter()
{
	entityType = ET_EMITTER;
	
	type = EMITTER_FIRE;
	thinkTime = Math::rrand(1, 3);
	
	active = true;
}

Emitter::~Emitter()
{
}

void Emitter::loadAdditional(Properties *props)
{
	type = props->getInt("type", type);
	color = props->getColor("color");
	active = props->getInt("active", active);
	
	Math::limit(&color.color[0], 0.1, 1.0);
	Math::limit(&color.color[1], 0.1, 1.0);
	Math::limit(&color.color[2], 0.1, 1.0);
}

void Emitter::saveAdditional(Properties *props)
{
	props->setProperty("type", type);
	props->setProperty("color", color);
	props->setProperty("classname", type);
	props->setProperty("active", active);
}
