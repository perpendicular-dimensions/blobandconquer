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

#include "bioMechBlobs.h"

// ########################### General Stuff ################################

void generalBlobFirePanic()
{
	Vector target;

	float z = self->velocity.z;
	
	target = self->position;
	
	target.x += Math::rrand(-50, 50);
	target.y += Math::rrand(-50, 50);
	
	self->velocity = target;
	self->velocity -= self->position;
	self->velocity.normalize();
	self->velocity = self->velocity * 1.5;
	
	self->velocity.z = z;
			
	faceLocation((Unit*)self, target);
	
	self->thinkTime = Math::rrand(75, 150);
	
	if ((self->entityType == ET_BLOB) && (self->definition->type == BLOB_TEEKA) && ((rand() % 5) == 0))
	{
		self->flags &= ~EF_ONFIRE;
		self->thinkTime = 0;
		self->action = self->walk;
	}
}

void generalMechLargeExplosion()
{
	Vector v;

	for (int i = 0 ; i < 25 ; i++)
	{
		throwExplosionDebris(self->position);
	}
		
	self->flags |= EF_DEAD;
		
	for (int i = 0 ; i < 10 ; i++)
	{
		v = self->position;
		v.x += Math::rrand((int)self->boundingBox.mins.x, (int)self->boundingBox.maxs.x);
		v.y += Math::rrand((int)self->boundingBox.mins.y, (int)self->boundingBox.maxs.y);
		v.z += Math::rrand((int)self->boundingBox.mins.z, (int)self->boundingBox.maxs.z);
		doExplosion(v, 10, self);
	}
		
	audio->playSound(SND_LARGE_EXPLOSION, CH_ANY, camera->getSoundDistance(self->position));
	
	if (self->entityType != ET_BIOMECH)
	{
		return;
	}
		
	Unit *unit = (Unit*)self;
		
	if (unit->currentItem != NULL)
	{
		unit->currentItem->position = unit->position;
		
		unit->currentItem->setOwner(NULL);

		throwSpawnedItem(unit->currentItem);

		unit->currentItem = NULL;
	}
}

void generalMechDeathBoom()
{
	Unit *unit = (Unit*)self;

	if (unit->health > -100)
	{
		unit->health -= engine->getTimeDifference(TD_LOGIC);
		return;
	}

	unit->flags &= ~EF_DYING;
	unit->flags |= EF_DEAD;

	doExplosion(unit->position, 10, self);

	for (int i = 0 ; i < 10 ; i++)
	{
		throwExplosionDebris(self->position);
	}

	self->action = NULL;
	self->velocity.set(0, 0, 0);
}

void generalBlobDeathSplat()
{
	Vector adjustment;

	Unit *unit = (Unit*)self;
	
	if (unit->health > -100)
	{
		if ((rand() % 3) == 0)
		{
			adjustment = self->position;
			adjustment.x += Math::rrand(-2, 2);
			adjustment.y += Math::rrand(-2, 2);
			
			addBloodSplat(adjustment, 2);
		}
		
		unit->health -= engine->getTimeDifference(TD_LOGIC);
		return;
	}
	
	if ((!(unit->flags & EF_ONGROUND)) && (unit->contentType == (CONTENTS_SOLID|CONTENTS_AIR)))
	{
		unit->helpless = 25;
		unit->flags &= ~EF_BOUNCES;
		return;
	}

	self->flags &= ~EF_DYING;
	self->flags |= EF_DEAD;

	if (unit->liquidLevel < 2)
	{
		if (game->bloodPolicy != BLOOD_POLICY_OFF)
		{
			audio->playSound(SND_SPLAT, (self == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(self->position));
			addBloodSplat(self->position, Math::rrand(12, 14));
		}
		else
		{
			audio->playSound(SND_POP, (self == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(self->position));
		}
		
		int r = Math::rrand(5, 9);
		
		for (int i = 0 ; i < r ; i++)
		{
			adjustment = self->position;
			adjustment.x += Math::rrand(-15, 15);
			adjustment.y += Math::rrand(-15, 15);
			addBloodSplat(adjustment, Math::rrand(2, 4));
		}
	}
	else
	{
		if (game->bloodPolicy != BLOOD_POLICY_OFF)
		{
			audio->playSound(SND_SPLAT_UNDERWATER, (self == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(self->position));
		}
		else
		{
			audio->playSound(SND_POP, (self == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(self->position));
		}
	}

	if (game->bloodPolicy != BLOOD_POLICY_OFF)
	{
		for (int i = 0 ; i < 25 ; i++)
		{
			throwFleshChunk(self->position, (unit->liquidLevel == 2));
		}
	}
	else
	{
		throwNBFleshChunk(self->position);
	}

	self->action = NULL;
	self->velocity.set(0, 0, 0);
}

void generalEnemyLookAround()
{
	Vector target;

	Unit *unit = (Unit*)self;
	
	/*
	if (Math::getDistance(unit->lastTargetPosition, unit->position) > 2)
	{
		// haven't gone anywhere!
		if (Math::getDistance(self->oldPosition, unit->position) < 2)
		{
			self->velocity.x = Math::rrand(-20, 20);
			self->velocity.y = Math::rrand(-20, 20);
			self->velocity.x /= 35;
			self->velocity.y /= 35;
			self->thinkTime = Math::rrand(25, 150);

			angle = atan2(self->velocity.y, self->velocity.x);
			self->rotation.x = 90 + toDegrees(angle);
			
			return;
		}
		
		unit->velocity = unit->lastTargetPosition;
		unit->velocity -= unit->position;
		unit->velocity.normalize();
		
		unit->velocity.x /= 2;
		unit->velocity.y /= 2;
		
		if (!(unit->flags & EF_WEIGHTLESS))
		{
			unit->velocity.z = 0;
		}
		
		angle = atan2(self->velocity.y, self->velocity.x);
		self->rotation.x = 90 + toDegrees(angle);
		
		self->thinkTime = 25;
		
		self->oldPosition = self->position;
		
		return;
	}
	*/
	
	unit->lastTargetPosition = self->position;

	switch (rand() % 25)
	{
		case 0:
			target = self->position;
			target.x += Math::rrand(-50, 50);
			target.y += Math::rrand(-50, 50);
			
			if (unit->flags & EF_SWIMS)
			{
				target.z += Math::rrand(-50, 50);
			}
			
			self->velocity = target;
			self->velocity -= self->position;
			self->velocity.normalize();
			self->velocity = self->velocity * 0.25;
			
			faceLocation((Unit*)self, target);
			
			self->thinkTime = Math::rrand(300, 500);

			break;

		case 1:
			self->currentAnim = NULL;
			self->velocity.x = 0;
			self->velocity.y = 0;
			self->rotation.x = Math::rrand(0, 360);
			self->thinkTime = Math::rrand(100, 300);
			break;

		default:
			self->thinkTime = Math::rrand(1, 10);
			break;
	}
}

void bioMechMoveToTarget()
{
	Unit *unit = (Unit*)self;
	
	enemyMoveToTarget(unit);
	unit->thinkTime = Math::rrand(25, 75);
	faceTarget(unit);
}

void bioMechStrafeAtTarget()
{
	Unit *unit = (Unit*)self;
	unit->action = unit->attack;
	faceTarget(unit);
}

void bioMechHaltAndAttackTarget()
{
	Unit *unit = (Unit*)self;
	
	unit->currentAnim = NULL;
	unit->velocity.x = 0;
	unit->velocity.y = 0;
	unit->thinkTime = Math::rrand(0, 20);
	faceTarget(unit);
	unit->action = unit->attack;
}

void bioMechDodgeAroundTarget()
{
	Vector target;

	Unit *unit = (Unit*)self;
	
	float z = unit->velocity.z;
	
	target = unit->target->position;
	target.x += Math::rrand(-50, 50);
	target.y += Math::rrand(-50, 50);
			
	if (unit->flags & EF_SWIMS)
	{
		target.z += Math::rrand(-50, 50);
	}
			
	unit->velocity = target;
	unit->velocity -= unit->position;
	unit->velocity.normalize();
	unit->velocity = unit->velocity * 1.25;
	
	float speed = Math::rrand(150, 400);
	speed /= 750;
			
	faceLocation(unit, target);
	
	unit->velocity.x *= speed;
	unit->velocity.y *= speed;
	
	if ((!(unit->flags & EF_WEIGHTLESS)) && (!(unit->flags & EF_SWIMS)))
	{
		unit->velocity.z = z;
	}
			
	unit->thinkTime = Math::rrand(25, 75);
}

void bioMechJumpAndAttackTarget()
{
	Unit *unit = (Unit*)self;
	
	if (unit->flags & EF_ONGROUND)
	{
		unit->velocity.z = 1.25;
		unit->thinkTime = 50;
		unit->action = &bioMechStrafeAtTarget;
		faceTarget(unit);
	}
}

void bioMechSearch()
{
	Unit *unit = (Unit*)self;
	
	getAnimation("stand", unit);
	
	if (unit->target != NULL)
	{
		unit->lastTargetPosition = unit->target->position;
	}
	
	if (unit->flags & EF_NOTARGET)
	{
		generalEnemyLookAround();
		return;
	}
	
	if (!enemySightTarget(unit, 850))
	{
		generalEnemyLookAround();
		return;
	}
	
	// we now know we have a target... Determine whether we'll immediately attack or move around
	
	int r = rand() % 5;
	
	if (r <= 1)
	{
		bioMechMoveToTarget();
		return;
	}
	
	if (!enemySightTarget(unit, 200 + (75 * mission->enemyLevel)))
	{
		unit->currentAnim = NULL;
		generalEnemyLookAround();
		return;
	}
	
	r = Math::rrand(0, 10 + (15 * mission->enemyLevel));
	
	if (r < 10)
	{
		unit->currentAnim = NULL;
		bioMechHaltAndAttackTarget();
	}
	else if (r < 25)
	{
		bioMechDodgeAroundTarget();
	}
	else if (r < 50)
	{
		bioMechStrafeAtTarget();
	}
	else 
	{
		bioMechJumpAndAttackTarget();
	}
}

void bioMechReload()
{
	audio->playSound(SND_PISTOL_RELOAD, CH_ANY, camera->getSoundDistance(self->position));
	self->thinkTime = 100;
	self->action = &bioMechSearch;
}

void generalUnitDropWeaponAndItems()
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

	if ((unit->currentWeapon->type == WEP_PISTOL) || (unit->currentWeapon->type == WEP_GRENADE) || (unit->currentWeapon->type == WEP_MINI_GRENADES))
	{
		unit->currentWeapon = getRandomPlayerAmmoType();
	}
	
	addHelperItems(1, unit->currentWeapon, unit->position);

	unit->currentWeapon = NULL;

	if (unit->currentItem != NULL)
	{
		unit->currentItem->position = unit->position;
		
		unit->currentItem->setOwner(NULL);

		throwSpawnedItem(unit->currentItem);

		unit->currentItem = NULL;
	}
}

void onFireBlobDie()
{
	Vector velocity;
	float size, x, y, z;

	self->flags |= EF_DEAD;
	
	if (game->bloodPolicy == BLOOD_POLICY_OFF)
	{
		throwNBFleshChunk(self->position);
		audio->playSound(SND_POP, (self == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(self->position));
		return;
	}
	
	for (int i = 0 ; i < 50 ; i++)
	{
		size = Math::rrand(10, (150 - i));
		size *= 0.01;
		
		x = Math::rrand(-25, 25);
		y = Math::rrand(-25, 25);
		
		x *= 0.01;
		y *= 0.01;
		z = 0;
		
		velocity.set(x, y, z);
		addGeneralDebris(self->position, velocity, Math::rrand(200, 500), "textures/game/blood03.png", size, true);
	}
	
	addDebrisEmitter(self->position);
	
	audio->playSound(SND_SPLAT_UNDERWATER, (self == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(self->position));
	
	addBloodSplat(self->position, 30);
}

void frozenBlobDie()
{
	Vector velocity;
	float size, x, y, z;
	
	self->flags |= EF_DEAD;
	
	for (int i = 0 ; i < 25 ; i++)
	{
		size = Math::rrand(10, 150 - (i * 2));
		size *= 0.01;
		
		x = Math::rrand(-25, 25);
		y = Math::rrand(-25, 25);
		z = Math::rrand(0, 100);
		
		x *= 0.01;
		y *= 0.01;
		z *= 0.01;
		
		velocity.set(x, y, z);
		addGeneralDebris(self->position, velocity, Math::rrand(500, 1500), "textures/game/iceBlock.jpg", size, true);
	}
	
	audio->playSound(SND_ICE_SHATTER, CH_ANY, camera->getSoundDistance(self->position));
}

void generalBlobDie()
{
	Unit *unit = (Unit*)self;
	
	if (unit->flags & EF_FROZEN)
	{
		frozenBlobDie();
		return;
	}
	
	if (unit->flags & EF_ONFIRE)
	{
		onFireBlobDie();
		return;
	}
	
	if (unit->flags & EF_WRAPPED)
	{
		unit->mental = MAX_MENTAL;
		generalUnitWrapped();
	}
	
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

	generalUnitDropWeaponAndItems();

	unit->flags &= ~EF_WEIGHTLESS;
}

// ########################### Machine Gun Blob Stuff ################################

void machineGunBlobAttack()
{
	//debug(("machineGunBlobAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &machineGunBlobAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(25, 75);

		if ((rand() % 5) == 0)
		{
			self->thinkTime = 100;
			self->action = &bioMechReload;
		}
	}
}

void machineGunBlobPreAttack()
{
	//debug(("machineGunBlobPreAttack()\n"));

	self->custom = Math::rrand(3, 12);
	self->thinkTime = 0;
	self->action = &machineGunBlobAttack;
}

void machineGunBlobInit(Unit *unit)
{
	//debug(("machineGunBlobInit()\n"));

	unit->walk = &bioMechSearch;
	unit->attack = &machineGunBlobPreAttack;
	unit->die = &generalBlobDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ########################### Pistol Blob Stuff ################################

void pistolBlobAttack()
{
	//debug(("pistolBlobAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &pistolBlobAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(10, 50);
		
		if ((rand() % 5) == 0)
		{
			self->thinkTime = 100;
			self->action = &bioMechReload;
		}
	}
}

void pistolBlobPreAttack()
{
	//debug(("pistolBlobPreAttack()\n"));

	self->custom = Math::rrand(1, 2);
	self->thinkTime = 0;
	self->action = &pistolBlobAttack;
}

void pistolBlobInit(Unit *unit)
{
	//debug(("pistolBlobInit()\n"));

	unit->walk = &bioMechSearch;
	unit->attack = &pistolBlobPreAttack;
	unit->die = &generalBlobDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ########################### Shotgun Blob Stuff ################################

void shotGunBlobAttack()
{
	//debug(("shotGunBlobAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &shotGunBlobAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(25, 75);
		
		if ((rand() % 5) == 0)
		{
			self->thinkTime = 100;
			self->action = &bioMechReload;
		}
	}
}

void shotGunBlobPreAttack()
{
	self->custom = 1;
	self->thinkTime = 0;
	self->action = &shotGunBlobAttack;
}

void shotGunBlobInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->attack = &shotGunBlobPreAttack;
	unit->die = &generalBlobDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ############################ Grenade Blob Stuff ###################################

void grenadeBlobAttack()
{
	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	attackTarget(unit);

	if (--unit->custom == 0)
	{
		self->thinkTime = Math::rrand(100, 200);
		self->action = &bioMechSearch;
		
		if ((rand() % 5) == 0)
		{
			self->thinkTime = 100;
			self->action = &bioMechReload;
		}
	}
}

void grenadeBlobPreAttack()
{
	if ((rand() % 4) > 1)
	{
		((Unit*)self)->currentWeapon = &entityManager->weaponDef[WEP_GRENADE];
		self->custom = Math::rrand(1, 3);
		self->action = grenadeBlobAttack;
	}
	else
	{
		((Unit*)self)->currentWeapon = &entityManager->weaponDef[WEP_PISTOL];
		self->custom = Math::rrand(1, 2);
		self->action = pistolBlobAttack;
	}
}

void grenadeBlobInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->attack = &grenadeBlobPreAttack;
	unit->die = &generalBlobDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ########################## Plasma pistol blob ###################

void plasmaPistolBlobInit(Unit *unit)
{
	//debug(("pistolBlobInit()\n"));

	unit->walk = &bioMechSearch;
	unit->attack = &pistolBlobPreAttack;
	unit->die = &generalBlobDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ########################### Aqua Blob Stuff ################################

void aquaBlobAttack()
{
	//debug(("pistolBlobAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &aquaBlobAttack;

	attackTarget(unit);

	self->action = &bioMechSearch;
	self->thinkTime = Math::rrand(100, 200);
	
	if ((rand() % 5) == 0)
	{
		self->thinkTime += 100;
		self->action = &bioMechReload;
	}
}

void aquaBlobPreAttack()
{
	//debug(("pistolBlobPreAttack()\n"));

	self->thinkTime = 0;
	self->action = &aquaBlobAttack;
}

void aquaBlobInit(Unit *unit)
{
	//debug(("pistolBlobInit()\n"));

	unit->walk = &bioMechSearch;
	unit->attack = &aquaBlobPreAttack;
	unit->die = &generalBlobDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}
