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

#include "eyeDroids.h"

// ###################### General Stuff ############################

void generalDroidDie()
{
	((Unit*)self)->health = Math::rrand(-25, 0);
	self->velocity.z = 0;
	
	((Unit*)self)->helpless = 200;

	audio->playSound(Math::rrand(SND_DROID_DEATH1, SND_DROID_DEATH3), CH_ANY, camera->getSoundDistance(self->position));

	generalUnitDropWeaponAndItems();

	self->flags &= ~EF_WEIGHTLESS;
}

void generalDroidTouch(Entity *other)
{
	if (((Unit*)self)->helpless == 0)
	{
		return;
	}
	
	if (other != NULL)
	{
		return;
	}
	
	self->health = -100;
}

// ###################### Pistol Droids ############################

void pistolDroidAttack()
{
	//debug(("pistolDroidAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &pistolDroidAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(10, 50);
	}
}

void pistolDroidPreAttack()
{
	//debug(("pistolDroidPreAttack()\n"));

	// move in a random direction before firing...
	self->velocity.x = Math::rrand(-4, 4);
	self->velocity.y = Math::rrand(-4, 4);
	self->velocity.z = Math::rrand(-4, 4);

	self->velocity.x *= 0.01;
	self->velocity.y *= 0.01;
	self->velocity.z *= 0.01;

	self->custom = Math::rrand(1, 2);
	self->thinkTime = 0;
	self->action = &pistolDroidAttack;
}

void pistolDroidInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->touch = &generalDroidTouch;
	unit->attack = &pistolDroidPreAttack;
	unit->die = &generalDroidDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ###################### Machine Gun Droids ############################

void machineGunDroidAttack()
{
	//debug(("machineGunDroidAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}
	
	self->thinkTime = 0;
	self->action = &machineGunDroidAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(10, 50);
	}
}

void machineGunDroidPreAttack()
{
	//debug(("machineGunDroidPreAttack()\n"));

	self->custom = Math::rrand(4, 12);
	self->thinkTime = 0;
	self->action = &machineGunDroidAttack;
}

void machineGunDroidInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->touch = &generalDroidTouch;
	unit->attack = &machineGunDroidPreAttack;
	unit->die = &generalDroidDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ###################### Shotgun Droids ############################

void shotGunDroidAttack()
{
	//debug(("shotGunBlobAttack()\n"));

	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &shotGunDroidAttack;

	attackTarget(unit);
	
	unit->reload = Math::rrand(25, 50);

	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(5, 25);
	}
}

void shotGunDroidPreAttack()
{
	self->custom = Math::rrand(1, 3);
	self->thinkTime = 0;
	self->action = &shotGunDroidAttack;
}

void shotGunDroidInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->touch = &generalDroidTouch;
	unit->attack = &shotGunDroidPreAttack;
	unit->die = &generalDroidDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// ###################### Grenade Droids ############################

void grenadeGunDroidUziAttack()
{
	Unit *unit = (Unit*)self;
	
	unit->currentWeapon = &entityManager->weaponDef[WEP_UZI];

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}
	
	attackTarget(unit);
	
	if (--self->custom == 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(5, 25);
	}
}

void grenadeGunDroidGrenadeAttack()
{
	Unit *unit = (Unit*)self;
	
	unit->currentWeapon = &entityManager->weaponDef[WEP_MINI_GRENADES];

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}
	
	attackTarget(unit);
	
	if (--self->custom <= 0)
	{
		self->action = &bioMechSearch;
		self->thinkTime = Math::rrand(200, 300);
	}
}

void grenadeGunDroidPreAttack()
{
	if ((rand() % 4) == 0)
	{
		self->custom = Math::rrand(6, 10);
		self->action = grenadeGunDroidUziAttack;
	}
	else
	{
		
		self->custom = Math::rrand(3, 4);
		self->action = grenadeGunDroidGrenadeAttack;
	}
	
	self->thinkTime = 0;
}

void grenadeDroidInit(Unit *unit)
{
	unit->walk = &bioMechSearch;
	unit->touch = &generalDroidTouch;
	unit->attack = &grenadeGunDroidPreAttack;
	unit->die = &generalDroidDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}

// =========== Black Eye Droids ============

void blackDroidEnergyTouch(Entity *other)
{
	if (other == NULL)
	{
		return;
	}
	
	if (other != entityManager->bossList.getFirstElement())
	{
		return;
	}
	
	Boss *boss = (Boss*)other;
	
	boss->energyCharge++;
	
	if (boss->energyCharge < 15)
	{
		boss->velocity.set(0, 0, 0);
		boss->thinkTime = 100;
	}
	
	self->health = 0;
	self->touch = NULL;
	
	Particle *particle = particleManager->spawnParticle();
	particle->position = self->position;
	particle->life = 100;
	particle->size = 25;
	
	audio->playSound(SND_GALDOV_ENERGY, CH_ANY, camera->getSoundDistance(self->position));
}

void blackDroidEnergyThink()
{
	Boss *boss = (Boss*)entityManager->bossList.getFirstElement();
	
	if (boss->flags & EF_VANISHED)
	{
		self->health = 0;
		return;
	}
	
	boss->velocity = boss->velocity * 0.99;

	Vector targetSpeed = boss->position;
	targetSpeed -= self->position;
	targetSpeed.normalize();
	
	self->velocity.x += (targetSpeed.x * engine->getTimeDifference(TD_LOGIC));
	self->velocity.y += (targetSpeed.y * engine->getTimeDifference(TD_LOGIC));
	self->velocity.z += (targetSpeed.z * engine->getTimeDifference(TD_LOGIC));
	
	Math::limit(&self->velocity.x, -self->custom, self->custom);
	Math::limit(&self->velocity.y, -self->custom, self->custom);
	Math::limit(&self->velocity.z, -self->custom, self->custom);
}

void blackDroidReleaseAndDie()
{
	Bullet *bullet;
	
	for (int i = 0 ; i < 5 ; i++)
	{
		bullet = entityManager->spawnBullet(WEP_HOMING_ENERGY);
		bullet->position = self->position;
		bullet->health = RAND_MAX;
		bullet->action = blackDroidEnergyThink;
		bullet->touch = blackDroidEnergyTouch;
		bullet->thinkTime = Math::rrand(100, 200);
		bullet->velocity.set(Math::rrand(-25, 25), Math::rrand(-25, 25), Math::rrand(50, 100));
		bullet->velocity.x /= 100;
		bullet->velocity.y /= 100;
		bullet->velocity.z /= 100;
		bullet->custom = Math::rrand(50, 100);
		bullet->custom /= 100;
		bullet->draw = drawBullet;
	}
	
	generalDroidDie();
	self->die = &generalDroidDie;
	self->custom = 0;
}

void blackDroidWait()
{
	Unit *unit = (Unit*)self;

	unit->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (unit->contentType & CONTENTS_SLIME)
	{	
		blackDroidReleaseAndDie();
	}
	
	if (unit->contentType & CONTENTS_LAVA)
	{
		unit->custom = 0;
	}
	
	if (unit->custom <= 0)
	{
		generalDroidDie();
	}
}

void blackDroidPreAttack()
{
	Unit *unit = (Unit*)self;

	if ((rand() % 5) > 0)
	{
		unit->currentWeapon = &entityManager->weaponDef[WEP_PLASMA_RIFLE];
		unit->custom = Math::rrand(3, 7);
		unit->action = machineGunDroidAttack;
	}
	else
	{
		unit->currentWeapon = &entityManager->weaponDef[WEP_MINI_GRENADES];
		unit->custom = Math::rrand(2, 4);
		unit->action = grenadeGunDroidGrenadeAttack;
	}
	
	self->thinkTime = 0;
}

void blackDroidDie()
{
	Unit *unit = (Unit*)self;

	unit->flags &= ~EF_WEIGHTLESS;
	
	unit->helpless = 0;
	unit->rotation.set(30, 30, 0);
	
	unit->currentWeapon = &entityManager->weaponDef[WEP_PLASMA_PISTOL];
	generalUnitDropWeaponAndItems();
	unit->dead = false;
	unit->flags &= ~EF_DYING;
	unit->flags &= ~EF_DEAD;
	unit->flags &= ~EF_BOUNCES;
	unit->flags |= EF_ONFIRE;
	unit->custom = 650;
	
	unit->action = blackDroidWait;
	unit->walk = blackDroidWait;
	unit->attack = blackDroidWait;
	unit->die = blackDroidWait;
}

void blackDroidInit(Unit *unit, BossDef *bossDef)
{
	unit->walk = &bioMechSearch;
	unit->touch = &generalDroidTouch;
	unit->attack = &blackDroidPreAttack;
	unit->die = &blackDroidDie;
	unit->draw = &drawUnit;
	
	unit->health = bossDef->health;
	unit->maxHealth = bossDef->health;
	unit->shield = bossDef->shield;
	unit->maxShield = bossDef->shield;
	
	unit->currentWeapon = &entityManager->weaponDef[WEP_PLASMA_RIFLE];

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
}
