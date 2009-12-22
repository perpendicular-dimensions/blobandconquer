/*
Copyright (C) 2007 Parallel Realities

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

#include "ambush.h"

void galdovAmbushSearch();
void amushCloneInit(Boss *boss);

void galdovAmushSpawnClone()
{
	if ((self->name != "Galdov") || (entityManager->bossList.getSize() >= 4))
	{
		return;
	}

	int p;
			
	Vector wantedPosition = self->position;
	
	p = (Math::rrand(-2, 2) * 8);
	wantedPosition.x += p;
	
	p = (Math::rrand(-2, 2) * 8);
	wantedPosition.y += p;
	
	bsp->traceBox(wantedPosition, wantedPosition, self->boundingBox.mins, self->boundingBox.maxs);
	
	if (tracer->outputAllSolid)
	{
		return;
	}
	
	debug(("galdovAmushSpawnClone() %d -> %d\n", entityManager->bossList.getSize(), entityManager->bossList.getSize() + 1));
	
	Boss *boss = entityManager->spawnBoss();
	
	amushCloneInit(boss);
	
	boss->position = wantedPosition;
	
	addTeleportParticles(boss->position);
	
	audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
	audio->playSound(SND_GALDOV_LAUGH, CH_ANY, camera->getSoundDistance(self->position));
}

void galdovAmbushRetreat()
{
	if (self->flags & EF_DEAD)
	{
		return;
	}

	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		addTeleportParticles(boss->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
		boss->flags |= EF_DEAD;
		boss->health = 0;
	}
	
	self->velocity.set(0, 0, 0);
	self->flags |= EF_WEIGHTLESS;
	self->health = 1;
	self->flags &= ~EF_DEAD;
	
	game->setMissionStatus(MISSION_COMPLETE);
	game->changingLevel = true;
	game->targetMission = mission->getNextMissionName();
}

void galdovAmbushReactToBullet(Bullet *bullet)
{
	Boss *boss = (Boss*)self;
	
	float before = boss->getHealthPercent();
	
	if (bullet->definition->type != WEP_SHOTGUN)
	{
		boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
	}
	else
	{
		boss->applyDamage(1);
	}
	
	float after = boss->getHealthPercent();
	
	for (int i = 25 ; i < 75 ; i += 25)
	{
		if ((before > i) && (after <= i))
		{
			galdovAmushSpawnClone();
		}
	}
	
	addSparkParticle(bullet->position);
	audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
	bullet->health = 0;
	bullet->touch = NULL;
	
	if ((boss->health <= 0) && (!(boss->flags & EF_DEAD)))
	{
		if (boss->name != "Galdov")
		{
			addTeleportParticles(boss->position);	
			audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
			audio->playSound(SND_GALDOV_LAUGH, CH_ANY, camera->getSoundDistance(self->position));
			boss->flags |= EF_DEAD;
		}
		else
		{
			boss->health = 1;
		
			galdovAmbushRetreat();
		}
	}
}

void galdovAmbushReactToDamage(Entity *owner, float damage)
{
}

void amushCloneInit(Boss *boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "Galdov Clone";
	boss->entityType = ET_BOSS;
	
	boss->walk = &galdovAmbushSearch;
	boss->attack = &galdovAmbushSearch;
	boss->die = &galdovAmbushSearch;
	boss->reactToBullet = galdovAmbushReactToBullet;
	boss->reactToDamage = galdovAmbushReactToDamage;
	boss->draw = &drawBoss;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	boss->gunPosition[0] = bossDef->gunPosition[0];
	boss->gunPosition[1] = bossDef->gunPosition[1];
	
	boss->health = boss->maxHealth = 50;
	boss->shield = boss->maxShield = 0;
	boss->flags = boss->definition->flags;
	
	boss->showHealthData = true;

	boss->thinkTime = 0;
	boss->custom = 0;

	boss->action = galdovAmbushSearch;
}

void galdovAmushReset()
{
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		boss->flags |= EF_WEIGHTLESS;
		boss->action = galdovAmbushSearch;
		boss->thinkTime = 0;
		boss->touch = NULL;
		boss->custom = 0;
	}
}

void galdovAmbushGroundAttack()
{
	Boss *boss = (Boss*)self;
	
	if (boss->energyCharge <= 0)
	{
		boss->flags |= EF_WEIGHTLESS;
		boss->touch = NULL;
		boss->action = &galdovAmbushSearch;
		return;
	}
	
	boss->custom -= engine->getTimeDifference(TD_LOGIC);
	
	boss->rotation.x += engine->getTimeDifference(TD_LOGIC);

	if (boss->custom <= 0)
	{	
		self->velocity = player->position;
		self->velocity -= self->position;
		self->velocity.normalize();
		
		self->velocity.x *= Math::rrand(4, 7);
		self->velocity.y *= Math::rrand(4, 7);
		
		boss->custom = Math::rrand(25, 75);
		
		boss->energyCharge--;
		
		audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(self->position));
	}
	
	Particle *particle = particleManager->spawnParticle();
	particle->type = PT_NORMAL;
	particle->position = boss->position;
	particle->life = 125;
	particle->size = 5;
	particle->color = GLColor::skyBlue;
}

void galdovAmbushTackle(Entity *other)
{
	if (other == NULL)
	{
		// hit a wall. Stop moving...
		if (tracer->outputNormal->z != 1.0)
		{
			self->custom = 0;
		}
		return;
	}
	
	if (other->entityType == ET_BLOB)
	{
		Unit *unit = (Unit*)other;
	
		if (unit->helpless == 0)
		{
			for (int i = 0 ; i < 25 ; i++)
			{
				throwExplosionDebris(self->position);
			}
				
			doExplosion(self->position, Math::rrand(5, 10), self);
			
			unit->velocity = self->velocity * 2;
			unit->velocity.z = 1.25;
				
			audio->playSound(SND_LARGE_EXPLOSION, CH_ANY, camera->getSoundDistance(self->position));
			audio->playSound(SND_GALDOV_LAUGH, CH_ANY, camera->getSoundDistance(self->position));
			
			galdovAmushReset();
			
			if (other == player)
			{
				unit->touch = bobStunnedTouch;
			}
		}
	}
	else if (other->entityType == ET_SUPPLY_CRATE)
	{
		other->health = 0;
	}
}

void galdovAmbushPreGroundAttack()
{
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		boss->flags &= ~EF_WEIGHTLESS;
		boss->action = galdovAmbushGroundAttack;
		boss->thinkTime = 100;
		boss->touch = galdovAmbushTackle;
		boss->custom = 0;
		boss->energyCharge = Math::rrand(8, 12);
	}
}

void galdovAmbushSearch()
{
	int r = rand() % 100;
	
	if (r < 15)
	{
		self->velocity = player->position;
		self->velocity.z += Math::rrand(50, 100);
		self->velocity -= self->position;
		self->velocity.normalize();
		self->velocity = self->velocity * 2;
		self->thinkTime = 100;
		
		self->action = galdovPreMachineGunAttack;
	}
	else if (r < 45)
	{
		self->velocity.set(0, 0, 0);
		self->thinkTime = Math::rrand(50, 200);
	}
	else if (r < 75)
	{
		self->velocity.set(0, 0, 0);
		
		if ((rand() % 3) == 0)
		{
			self->action = galdovPreMachineGunAttack;
		}
		
		self->thinkTime = Math::rrand(100, 200);
	}
	else if (r < 85)
	{
		((Boss*)self)->energyCharge = rand() % 15;
		self->thinkTime = 0;
	}
	else if (r < 93)
	{
		galdovAmushSpawnClone();
	}
	else if (self->name == "Galdov")
	{
		self->action = galdovAmbushPreGroundAttack;
	}
}

void amushBossInit(Boss *boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "Galdov";
	boss->entityType = ET_BOSS;
	
	boss->walk = &galdovAmbushSearch;
	boss->attack = &galdovAmbushSearch;
	boss->die = &galdovAmbushSearch;
	boss->reactToBullet = galdovAmbushReactToBullet;
	boss->reactToDamage = galdovAmbushReactToDamage;
	boss->draw = &drawBoss;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	boss->gunPosition[0] = bossDef->gunPosition[0];
	boss->gunPosition[1] = bossDef->gunPosition[1];
	
	boss->health = boss->maxHealth = bossDef->health;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->health = boss->maxHealth = 500;
	
	boss->showHealthData = true;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = galdovAmbushSearch;
}
