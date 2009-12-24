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

#include "weapons.h"

void fireWeapon(Unit *owner, WeaponDef *definition, Vector origin, Vector destination)
{
	int scatterAdjustment, scatterX, scatterY;
	
	if (owner == player)
	{
		game->alterStat(_("Bullets Fired"), 1);
	}
	
	switch (definition->type)
	{
		case WEP_SHOTGUN:
			for (int i = 0 ; i < definition->damage ; i++)
			{
				addPellet(owner, definition, origin, destination);
			}
			break;
		
		case WEP_GRENADE:
			throwGrenade(owner, origin, destination);
			break;
				
		case WEP_GRENADE_LAUNCHER:
		case WEP_CLUSTER_LAUNCHER:
			addLauncherShot(owner, definition, origin, destination);
			break;
			
		case WEP_ROCKET_LAUNCHER:
			addRocket(owner, origin, destination);
			break;
			
		case WEP_UZI:
			scatterAdjustment = 150;
			scatterX = Math::rrand(-scatterAdjustment, scatterAdjustment);
			scatterY = Math::rrand(-scatterAdjustment, scatterAdjustment);
			scatterX /= 100;
			scatterY /= 100;
			destination.x += scatterX;
			destination.y += scatterY;
			addNormalBullet(owner, definition, origin, destination);
			break;
			
		case WEP_MINI_MINE:
			addMiniMine(owner, origin, destination);
			break;
			
		case WEP_ICE_GUN:
			scatterAdjustment = (int)(Math::getDistance(owner->position.x, owner->position.y, destination.x, destination.y) / 35);
			destination.x += Math::rrand(-scatterAdjustment, scatterAdjustment);
			destination.y += Math::rrand(-scatterAdjustment, scatterAdjustment);
			addIceShot(owner, origin, destination);
			break;
			
		case WEP_FLAMETHROWER:
			scatterAdjustment = (int)(Math::getDistance(owner->position.x, owner->position.y, destination.x, destination.y) / 35);
			destination.x += Math::rrand(-scatterAdjustment, scatterAdjustment);
			destination.y += Math::rrand(-scatterAdjustment, scatterAdjustment);
			addFlame(owner, origin, destination);
			break;
			
		case WEP_DESIMILATOR:
			addDesimilatorBeam(owner, origin, destination);
			break;
		
		default:
			addNormalBullet(owner, definition, origin, destination);
			break;
	}
	
	if (definition->firingSound != SND_THROW)
	{
		audio->playSound(definition->firingSound, (owner == player) ? CH_PLAYER : CH_WEAPON, camera->getSoundDistance(origin));
	}
	else
	{
		audio->playSound(Math::rrand(SND_THROW1, SND_THROW2), (owner == player) ? CH_PLAYER : CH_WEAPON, camera->getSoundDistance(origin));
	}
	
	owner->reload = definition->reloadTime;
}
