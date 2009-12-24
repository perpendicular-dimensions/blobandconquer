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

Structure::Structure()
{
	boundingBox.mins.set(0, 0, 0);
	boundingBox.maxs.set(0, 0, 0);
	
	flags = EF_IMMORTAL+EF_NOCLIP+EF_WEIGHTLESS;
	
	speed = 0.5;
	
	active = true;
	
	hint = true;
	
	currentItem = NULL;
	
	isMissionTarget = false;
}

Structure::~Structure()
{
}

void Structure::loadAdditional(Properties *props)
{
	state = props->getInt("state", state);
	speed = props->getFloat("speed", speed);
	active = props->getInt("active", active);
	startPosition = props->getVector("startPosition", startPosition);
	endPosition = props->getVector("endPosition", endPosition);
	MDLModelName = props->getString("model", MDLModelName.getText());
	hint = props->getInt("hint", hint);
	isMissionTarget = props->getInt("isMissionTarget", isMissionTarget);
}

void Structure::saveAdditional(Properties *props)
{
	props->setProperty("state", state);
	props->setProperty("speed", speed);
	props->setProperty("active", active);
	props->setProperty("startPosition", startPosition.toString());
	props->setProperty("endPosition", endPosition.toString());
	props->setProperty("model", MDLModelName.getText());
	props->setProperty("hint", hint);
	props->setProperty("isMissionTarget", isMissionTarget);
}
