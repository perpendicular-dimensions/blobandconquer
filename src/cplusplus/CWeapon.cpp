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

Weapon::Weapon(long uniqueId)
{
	this->uniqueId = uniqueId;
	
	currentAmmo = -1;
	
	entityType = ET_WEAPON;
	
	boundingBox.boxType = BOX_HOLLOW;
	
	boundingBox.mins.set(-1, -1, -1);
	boundingBox.maxs.set(1, 1, 1);
	//updateBoundingBox();
}

Weapon::~Weapon()
{
}

void Weapon::loadAdditional(Properties *props)
{
	currentAmmo = props->getInt("currentAmmo", currentAmmo);
	
	if (currentAmmo == -1)
	{
		currentAmmo = ((WeaponDef*)definition)->clipSize / 4;
	}
	
	// ensure bad properties don't override
	entityType = ET_WEAPON;
}

void Weapon::saveAdditional(Properties *props)
{
	//props->setName("Weapon");
	
	props->setProperty("currentAmmo", currentAmmo);
}
