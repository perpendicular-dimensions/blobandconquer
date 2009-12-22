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

Switch::Switch(long uniqueId)
{
	this->uniqueId = uniqueId;

	entityType = ET_SWITCH;
	
	switchType = SWT_NORMAL;
	
	independent = false;
	
	active = false;
	
	puzzleType = Math::rrand(PUZZLE_TYPE_MASTERMIND, PUZZLE_TYPE_LASERGRID);
	puzzleSeed = Math::rrand(1000000, 9999999);
	
	model = NULL;
	texture[0] = texture[1] = NULL;
	
	altModel = false;
	
	rotation.z = 90;
	
	boundingBox.mins.set(-1, -3, 0);
	boundingBox.maxs.set(1, 3, 15);
}

Switch::~Switch()
{
}

void Switch::loadAdditional(Properties *props)
{
	active = props->getInt("active", active);
	switchType = props->getInt("switchType", switchType);
	requires = props->getString("requires", requires.getText());
	custom = props->getFloat("timeout", custom);
	independent = props->getInt("independent", independent);
	puzzleType = props->getInt("puzzleType", puzzleType);
	puzzleSeed = props->getInt("puzzleSeed", puzzleSeed);
	altModel = props->getInt("altModel", altModel);
	rotation.z -= props->getFloat("angle", 0);
	
	if (requires != "")
	{
		altModel = true;
	}
	
	if (altModel)
	{
		boundingBox.mins.set(-3, -3, 0);
		boundingBox.maxs.set(3, 3, 10);
	}
}

void Switch::saveAdditional(Properties *props)
{
	switch (switchType)
	{
		case SWITCH_DEMOLITION:
			props->name = "DemolitionPlate";
			break;
			
		case SWT_PRESSURE:
			props->name = "PressurePlate";
			break;
			
		default:
			props->name = "Switch";
			break;
	}
	
	props->setProperty("active", active);
	props->setProperty("switchType", switchType);
	props->setProperty("requires", requires.getText());
	props->setProperty("independent", independent);
	props->setProperty("puzzleType", puzzleType);
	props->setProperty("puzzleSeed", puzzleSeed);
	props->setProperty("altModel", altModel);
}
