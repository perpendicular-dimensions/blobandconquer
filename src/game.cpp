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

#include "game.h"

void addPlayerWeapon(int slot, int weaponType, int ammo)
{
	Properties weaponProps;
	
	weaponProps.setProperty("classname", weaponType);
	weaponProps.setProperty("currentAmmo", ammo);
	
	game->weapon[slot] = addWeapon(&weaponProps);
	game->weapon[slot]->setOwner(player);
}

void spawnBob()
{
	player = entityManager->spawnBlob(BLOB_BOB);
	player->setName("Bob");
	player->position.set(0, 0, 0);
	player->health = player->maxHealth = 25;
	
	player->die = &generalBlobDie;
	player->reload = 0;
	player->action = NULL;
	player->draw = drawUnit;
	
	addPlayerWeapon(0, WEP_PISTOL, 9999);
	//addPlayerWeapon(1, WEP_UZI, 9999);
	//addPlayerWeapon(2, WEP_SHOTGUN, 9999);
	addPlayerWeapon(3, WEP_GRENADE, 4);
	
	game->weaponIndex = 0;
	game->selectedWeapon = game->weapon[0];
	
	if (game->training)
	{
		player->flags |= EF_CANNOT_BE_KILLED;
	}
}

void newGame()
{
	game->clear();
	uiManager->clearWidgetList();
	entityManager->clear();
	
	player = NULL;
	
	srand(time(NULL));
}
