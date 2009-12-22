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

#include "spiderBlobs.h"

void spiderBlobDie3()
{
	Vector v;

	v = self->position;
	v.x += Math::rrand(-3, 3);
	v.y += Math::rrand(-3, 3);
	
	addSmokeParticle(v);
	
	if (game->bloodPolicy != BLOOD_POLICY_OFF)
	{
		for (int i = 0 ; i < 2 ; i++)
		{
			throwFleshChunk(self->position, false);
		}
	}
	else
	{
		throwNBFleshChunk(self->position);
	}
	
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		generalMechLargeExplosion();
	
		int r = Math::rrand(1, 4);
		
		WeaponDef *def;
		
		for (int i = 0 ; i < r ; i++)
		{
			def = getRandomPlayerAmmoType();
			addHelperItems(1, def, self->position);
		}
	}
}

void spiderBlobDie2()
{
	Vector v;

	v = self->position;
	
	v.x += Math::rrand((int)self->boundingBox.mins.x, (int)self->boundingBox.maxs.x);
	v.y += Math::rrand((int)self->boundingBox.mins.y, (int)self->boundingBox.maxs.y);
	v.z += Math::rrand((int)self->boundingBox.mins.z, (int)self->boundingBox.maxs.z);
	doExplosion(v, 10, self);
	
	if (game->bloodPolicy != BLOOD_POLICY_OFF)
	{
		for (int i = 0 ; i < 25 ; i++)
		{
			throwFleshChunk(self->position, false);
		}
	}
	else
	{
		throwNBFleshChunk(self->position);
	}
	
	for (int i = 0 ; i < 8 ; i++)
	{
		v = self->position;
		v.x += Math::rrand(-32, 32);
		v.y += Math::rrand(-32, 32);
		addBloodSplat(v, Math::rrand(4, 16));
	}
	
	self->thinkTime = Math::rrand(15, 30);
	
	if (--self->custom == 0)
	{
		self->action = &spiderBlobDie3;
		
		audio->playSound(SND_SPIDERBLOB_DIE, CH_ANY, camera->getSoundDistance(self->position));
		
		self->custom = 200;
	}
}
	
void spiderBlobDie1()
{
	self->health = 0;
	
	self->thinkTime = 0;
	
	self->action = spiderBlobDie2;
	
	self->custom = Math::rrand(3, 5);
	
	self->flags |= EF_DYING;
	self->flags &= ~EF_BOUNCES;
	
	audio->playSound(SND_SPIDERBLOB_DIE, CH_ANY, camera->getSoundDistance(self->position));
	
	self->velocity.x = self->velocity.y = 0;
}
		
void spiderBlobWebAttack()
{
	Unit *unit = (Unit*)self;
	
	if (unit->reload > 0)
	{
		return;
	}
	
	faceLocation(unit, unit->target->position);
	
	unit->velocity.x = unit->velocity.y = 0;
	
	Vector t = unit->target->position;
	
	if ((rand() % 3) == 0)
	{
		t = getLeadingTargetPosition(unit, unit->target);
	}
	
	addWeb(unit, unit->position, t);
	
	audio->playSound(SND_WEB_FIRE, CH_ANY, camera->getSoundDistance(self->position));
	
	unit->reload = 25;
	
	if (--unit->custom <= 0)
	{
		unit->action = unit->walk;
		unit->thinkTime = 100;
	}
}

void spiderBlobMortorAttack()
{
	Vector targetPosition;

	Unit *unit = (Unit*)self;
	
	unit->velocity.x = unit->velocity.y = 0;
	
	if (unit->reload > 0)
	{
		return;
	}
	
	targetPosition = unit->target->position;
	targetPosition.x += Math::rrand(-25, 25);
	targetPosition.y += Math::rrand(-25, 25);
	targetPosition.z = unit->position.z;
	addLauncherShot(self, &entityManager->weaponDef[WEP_GRENADE_LAUNCHER], unit->position, targetPosition);
	
	audio->playSound(SND_GRENADE_LAUNCHER, CH_ANY, camera->getSoundDistance(unit->position));
	
	unit->reload = 25;
	
	if ((--unit->custom) == 0)
	{
		unit->action = self->walk;
		unit->thinkTime = 250;
	}
}
		
void spiderBlobNormalAttack()
{
	Unit *unit = (Unit*)self;
	
	faceTarget(unit);
	
	if (unit->reload > 0)
	{
		return;
	}
	
	fireWeapon(unit, unit->currentWeapon, getGunPosition(unit, 0), unit->target->position);
	fireWeapon(unit, unit->currentWeapon, getGunPosition(unit, 1), unit->target->position);
	
	if ((unit->custom--) == 0)
	{
		unit->action = unit->walk;
		unit->thinkTime = 100;
	}
}
		
void spiderBlobAttack()
{
	Unit *unit = (Unit*)self;
	
	int r = rand() % (18 - mission->enemyLevel);
	
	if (unit->definition->type == NME_BABY_SPIDERBLOB)
	{
		// Can never fire mortor shots....
		r++;
	}
	
	if (r == 0)
	{
		unit->custom = Math::rrand(1, 2);
		unit->action = &spiderBlobMortorAttack;
	}
	else if (r <= 3)
	{
		unit->custom = Math::rrand(2, 3);
		unit->action = &spiderBlobWebAttack;
		
		if (unit->flags & EF_WRAPPED)
		{
			return;
		}
	}
	else
	{
		switch (unit->definition->type)
		{
			case NME_MACHINEGUN_SPIDERBLOB:
				unit->custom = Math::rrand(5, 8);
				break;
			
			case NME_SHOTGUN_SPIDERBLOB:
				unit->custom = Math::rrand(1, 3);
				break;
			
			case NME_PLASMA_SPIDERBLOB:
				unit->custom = Math::rrand(3, 5);
				break;
				
			case NME_BABY_SPIDERBLOB:
				unit->custom = Math::rrand(2, 3);
				break;
		}
		
		unit->action = &spiderBlobNormalAttack;
	}
	
	//unit->target = player;
}

void machineGunSpiderBlobInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->attack = &spiderBlobAttack;
	unit->die = &spiderBlobDie1;
	
	unit->currentAnimFrame = 0;
	unit->nextAnimFrame = 1;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->scale = Math::rrand(100, 125);
	unit->scale *= 0.01;
	unit->boundingBox.mins.x *= unit->scale;
	unit->boundingBox.mins.y *= unit->scale;
	unit->boundingBox.mins.z *= unit->scale;
	unit->boundingBox.maxs.x *= unit->scale;
	unit->boundingBox.maxs.y *= unit->scale;
	unit->boundingBox.maxs.z *= unit->scale;
}

void shotGunSpiderBlobInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->attack = &spiderBlobAttack;
	unit->die = &spiderBlobDie1;
	
	unit->currentAnimFrame = 0;
	unit->nextAnimFrame = 1;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->scale = Math::rrand(115, 150);
	unit->scale *= 0.01;
	unit->boundingBox.mins.x *= unit->scale;
	unit->boundingBox.mins.y *= unit->scale;
	unit->boundingBox.mins.z *= unit->scale;
	unit->boundingBox.maxs.x *= unit->scale;
	unit->boundingBox.maxs.y *= unit->scale;
	unit->boundingBox.maxs.z *= unit->scale;
}

void plasmaSpiderBlobInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->attack = &spiderBlobAttack;
	unit->die = &spiderBlobDie1;
	
	unit->currentAnimFrame = 0;
	unit->nextAnimFrame = 1;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->scale = Math::rrand(125, 175);
	unit->scale *= 0.01;
	unit->boundingBox.mins.x *= unit->scale;
	unit->boundingBox.mins.y *= unit->scale;
	unit->boundingBox.mins.z *= unit->scale;
	unit->boundingBox.maxs.x *= unit->scale;
	unit->boundingBox.maxs.y *= unit->scale;
	unit->boundingBox.maxs.z *= unit->scale;
}

// ========= Spider Blob Commander Only ===============

void babySpiderThreadCheck()
{
	Entity *owner = self->owner;
	
	if ((!(owner->flags & EF_GRAPPLING)) || (owner->health <= 0))
	{
		self->health = 0;
		return;
	}
}

void babySpiderBlobTouch(Entity *other)
{
	self->action = self->walk;
	self->thinkTime = 0;
	self->flags &= ~EF_GRAPPLING;
	self->flags &= ~EF_WEIGHTLESS;
	self->touch = NULL;
}

void babySpiderBlobSlideDown()
{
	self->velocity.x = self->velocity.y = 0;
	self->velocity.z = self->custom;
	self->flags &= ~EF_WEIGHTLESS;
}

void babySpiderBlobPreSlideDown()
{
	Unit *unit = (Unit*)self;
	
	Bullet *bullet = addGrapplingHook(unit, unit->position);
	bullet->velocity.set(0, 0, 0);
	bullet->action = babySpiderThreadCheck;
	unit->action = babySpiderBlobSlideDown;
	unit->custom = -Math::rrand(10, 150);
	unit->custom *= 0.01;
}

void babySpiderBlobInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->attack = &spiderBlobAttack;
	unit->die = &generalDroidDie;
	unit->touch = &babySpiderBlobTouch;
	
	unit->currentAnimFrame = 0;
	unit->nextAnimFrame = 1;

	unit->action = babySpiderBlobPreSlideDown;
	
	unit->scale = Math::rrand(50, 75);
	unit->scale *= 0.01;
	unit->boundingBox.mins.x *= unit->scale;
	unit->boundingBox.mins.y *= unit->scale;
	unit->boundingBox.mins.z *= unit->scale;
	unit->boundingBox.maxs.x *= unit->scale;
	unit->boundingBox.maxs.y *= unit->scale;
	unit->boundingBox.maxs.z *= unit->scale;
	
	unit->rotation.z = 180;
	
	self->thinkTime = 0;
}
