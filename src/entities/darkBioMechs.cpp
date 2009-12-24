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

#include "darkBioMechs.h"

void darkBlobThrowWeapon(Vector position, const char *weaponName)
{
	if ((rand() % 2) == 0)
	{
		return;
	}
	
	Weapon *weapon;
	WeaponDef *weaponDef;
	float clipSize;
	int minAmmo, maxAmmo;
	static Properties weaponProps;
	
	weaponProps.setProperty("classname", weaponName);
	weaponProps.setProperty("position", position);
	weapon = addWeapon(&weaponProps);
	weaponDef = (WeaponDef*)weapon->definition;
	clipSize = weaponDef->clipSize;
	minAmmo = (int)max(1, (clipSize * 0.25));
	maxAmmo = (int)max(2, (clipSize * 0.75));
	weapon->currentAmmo = Math::rrand(minAmmo, maxAmmo);
	
	if (weaponDef->type == WEP_GRENADE)
	{
		weapon->currentAmmo = 1;
	}
	
	throwSpawnedItem(weapon);
}

void darkMechDropWeaponAndItems()
{
	Unit *unit = (Unit*)self;

	if (!(unit->flags & EF_EXPLODES))
	{
		unit->action = &generalBlobDeathSplat;
	}
	else
	{
		unit->action = &generalMechDeathBoom;
	}

	if (unit->dead)
	{
		return;
	}
	
	static Properties itemProps;
		
	itemProps.setProperty("classname", "ITM_CHERRY");
	itemProps.setProperty("position", unit->position);
		
	int r;
	Item *item;
	
	r = rand() % 3;
			
	for (int i = 0 ; i < r ; i++)
	{
		item = addItem(&itemProps);
		item->itemPower = Math::rrand(1, item->itemPower);
		throwSpawnedItem(item);
	}
	
	darkBlobThrowWeapon(unit->position, "WEP_UZI");
	darkBlobThrowWeapon(unit->position, "WEP_SHOTGUN");
	
	if (mission->enemyLevel == 5)
	{
		if ((rand() % 2) == 0)
		{
			darkBlobThrowWeapon(unit->position, "WEP_PLASMA_RIFLE");
		}
	}
	else
	{
		darkBlobThrowWeapon(unit->position, "WEP_PLASMA_PISTOL");
	}
	
	darkBlobThrowWeapon(unit->position, "WEP_GRENADE");
	
	r = getAdjustedHealthChance(5, 1);
	
	if ((rand() % r) == 0)
	{
		itemProps.setProperty("classname", "ITM_MEDIPACK");
		itemProps.setProperty("position", unit->position);
		item = addItem(&itemProps);
		item->itemPower = Math::rrand(10, item->itemPower);
		throwSpawnedItem(item);
	}
	
	if (unit->currentItem != NULL)
	{
		unit->currentItem->position = unit->position;
		
		unit->currentItem->setOwner(NULL);

		throwSpawnedItem(unit->currentItem);

		unit->currentItem = NULL;
	}
}

void generalDarkMechDie()
{
	Unit *unit = (Unit*)self;
	
	unit->health = 0;
	unit->helpless = 200;
	
	if (unit->contentType & CONTENTS_SOLID)
	{
		self->velocity.z = Math::rrand(50, 100);
		self->velocity.z /= 100;
	}
	
	unit->thinkTime = 0;

	if (unit->liquidLevel != 2)
	{
		if (game->bloodPolicy != BLOOD_POLICY_OFF)
		{
			audio->playSound(Math::rrand(SND_DEATH1, SND_DEATH4), (unit == player) ? CH_PLAYER : CH_DEATH, camera->getSoundDistance(self->position));
		}
	}
	else if (unit->oxygen != 0)
	{
		audio->playSound(SND_DROWNING, (unit == player) ? CH_PLAYER : CH_DEATH, camera->getSoundDistance(self->position));
	}

	darkMechDropWeaponAndItems();

	unit->flags &= ~EF_WEIGHTLESS;
}

void darkBlobChaseTarget()
{
	Unit *unit = (Unit*)self;
	
	enemyMoveToTarget(unit);
	unit->thinkTime = Math::rrand(15, 45);
	faceTarget(unit);
}

void darkBioMechSearch()
{
	Unit *unit = (Unit*)self;
	
	if (unit->target != NULL)
	{
		unit->lastTargetPosition = unit->target->position;
	}
	
	if (!enemySightTarget(unit, 850))
	{
		generalEnemyLookAround();
		Math::limit(&self->thinkTime, 0, 150);
		return;
	}
	
	if (!enemySightTarget(unit, 550))
	{
		Math::limit(&self->thinkTime, 0, 50);
		return;
	}
	
	int r = rand() % 60;
	
	if (r < 25)
	{
		darkBlobChaseTarget();
	}
	else if (r < 35)
	{
		darkBlobChaseTarget();
		bioMechDodgeAroundTarget();
	}
	else if (r < 50)
	{
		darkBlobChaseTarget();
		bioMechStrafeAtTarget();
	}
	else 
	{
		darkBlobChaseTarget();
		bioMechJumpAndAttackTarget();
	}
}

// ########################### Dark Blob Stuff ################################

void darkBlobAttack()
{
	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	unit->thinkTime = 0;
	unit->action = &darkBlobAttack;

	if (unit->target == player)
	{
		if ((rand() % 5) == 0)
		{
			attackTargetLeading(unit);
		}
		else
		{
			attackTarget(unit);
		}
	}
	else
	{
		attackTarget(unit);
	}
	
	if (--unit->custom == 0)
	{
		self->thinkTime = Math::rrand(10, 25);
		self->action = &darkBioMechSearch;
	}
}

int getDarkBlobFireRate()
{
	int min = 0;
	int max = 0;
	
	Unit *unit = (Unit*)self;
	
	if (unit->currentWeapon == &entityManager->weaponDef[WEP_UZI])
	{
		min = 6;
		max = 12;
	}
	else if (unit->currentWeapon == &entityManager->weaponDef[WEP_SHOTGUN])
	{
		min = 1;
		max = 2;
	}
	else if (unit->currentWeapon == &entityManager->weaponDef[WEP_PLASMA_PISTOL])
	{
		min = 3;
		max = 5;
	}
	else if (unit->currentWeapon == &entityManager->weaponDef[WEP_PLASMA_RIFLE])
	{
		min = 3;
		max = 4;
	}
	else if (unit->currentWeapon == &entityManager->weaponDef[WEP_GRENADE])
	{
		min = 1;
		max = 2;
	}
	else
	{
		min = 2;
		max = 5;
	}
	
	return Math::rrand(min, max);
}

bool darkBlobSwitchWeapon(Unit *unit)
{
	bool swap = false;
	
	if ((rand() % 10) == 0)
	{
		swap = true;
	}
			 
	if (unit->currentWeapon == &entityManager->weaponDef[WEP_GRENADE])
	{
		swap = true;
	}
	
	if (unit->currentWeapon == &entityManager->weaponDef[WEP_PISTOL])
	{
		swap = true;
	}
		
	if (!swap)
	{
		return false;
	}
	
	switch (rand() % 3)
	{
		case 0:
			unit->currentWeapon = &entityManager->weaponDef[WEP_SHOTGUN];
			break;
				
		case 1:
			unit->currentWeapon = &entityManager->weaponDef[WEP_UZI];
			break;
				
		case 2:
			if (mission->enemyLevel == 5)
			{
				unit->currentWeapon = &entityManager->weaponDef[WEP_PLASMA_RIFLE];
			}
			else
			{
				unit->currentWeapon = &entityManager->weaponDef[WEP_PLASMA_PISTOL];
			}
			break;
	}
	
	return true;
}

void darkBlobPreAttack()
{
	Unit *unit = (Unit*)self;
	
	unit->thinkTime = 0;
	unit->action = &darkBlobAttack;
	
	if (!darkBlobSwitchWeapon(unit))
	{
		if ((rand() % 10) == 0)
		{
			unit->currentWeapon = &entityManager->weaponDef[WEP_GRENADE];
		}
	}
	
	unit->custom = getDarkBlobFireRate();
}

void darkBlobInit(Unit *unit)
{
	unit->walk = &darkBioMechSearch;
	unit->attack = &darkBlobPreAttack;
	unit->die = &generalDarkMechDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	darkBlobSwitchWeapon(unit);
}
