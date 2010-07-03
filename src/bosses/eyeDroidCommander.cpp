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

#include "eyeDroidCommander.h"

void eyeDroidCommanderSeek();
void eyeDroidCommanderAttack();
void eyeDroidCommanderRocketAttack();
void eyeDroidCommanderDie1();
void eyeDroidCommanderPlasmaAttack();

void eyeDroidCommanderDie3()
{
	self->velocity.x = self->velocity.y = 0;
	self->rotation.x += (3 * engine->getTimeDifference(TD_LOGIC));
	self->rotation.y += (3 * engine->getTimeDifference(TD_LOGIC));
	
	if (self->flags & EF_ONGROUND)
	{
		generalMechLargeExplosion();
		
		// kill all enemies
		for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
		{
			unit->health = 0;
		}
		
		Mix_FadeOutMusic(5000);
	}
}

void eyeDroidCommanderDie2()
{
	Vector v;

	self->velocity.set(Math::rrand(-1, 1), Math::rrand(-1, 1), 0.1);
	
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		self->flags &= ~EF_BOUNCES;
		self->flags &= ~EF_WEIGHTLESS;
		self->action = eyeDroidCommanderDie3;
	}
	
	int c = (int)self->custom;
	
	if (c > 100)
	{
		if ((c % 35) == 0)
		{
			v = self->position;
			v.x += Math::rrand(-5, 5);
			v.y += Math::rrand(-5, 5);
			v.z += Math::rrand(-5, 5);
			doExplosion(v, Math::rrand(5, 20), self);
			
			self->rotation.x = Math::rrand(0, 360);
			self->rotation.y = Math::rrand(0, 360);
		}
	}
}

void eyeDroidCommanderReactToDamage(Entity *owner, float amount)
{
	if ((self->flags & EF_IMMORTAL) || (self->health <= 0))
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	boss->applyDamage(amount);
	
	float percent = self->getHealthPercent();
			
	if (percent <= 0)
	{
		doExplosion(self->position, 20, self);
		self->flags |= EF_DYING;
		self->action = &eyeDroidCommanderDie1;
		self->custom = 250;
	}
	else if (percent <= 25)
	{
		self->thinkTime = 0;
	}
}

void eyeDroidCommanderReactToBullet(Bullet *bullet)
{
	if (self->health <= 0)
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	float percent = self->getHealthPercent();
	
	if (!(self->flags & EF_IMMORTAL))
	{
		if (boss->health > 0)
		{
			if (bullet->definition->type != WEP_SHOTGUN)
			{
				boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
			}
			else
			{
				boss->applyDamage(1);
			}
		}
		
		if (boss->shield <= 0)
		{
			addSparkParticle(bullet->position);
			audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
		}
		else
		{
			addShieldHitParticles(bullet->position);
			audio->playSound(SND_ELECTRIC_SPARK4, CH_ANY, camera->getSoundDistance(bullet->position));
		}
		
		WeaponDef *def = (WeaponDef*)bullet->definition;
	
		if ((def->type == WEP_PLASMA_PISTOL) || (def->type == WEP_PLASMA_RIFLE))
		{
			addPlasmaHitParticles(bullet->position);
			audio->playSound(SND_PLASMA_HIT, CH_ANY, camera->getSoundDistance(bullet->position));
		}
		
		bullet->health = 0;
		bullet->touch = NULL;
		
		if (self->health <= 0)
		{
			doExplosion(self->position, 20, self);
			self->flags |= EF_DYING;
			self->action = &eyeDroidCommanderDie1;
			self->custom = 250;
		}
		else if (percent <= 25)
		{
			self->thinkTime = 0;
		}
	}
	else
	{
		boss->shieldHit = 0.5;
		
		bullet->owner = self;
		
		if ((rand() % 4) < 3)
		{
			bullet->realVelocity.x = -bullet->realVelocity.x;
		}
		
		if ((rand() % 4) < 3)
		{
			bullet->realVelocity.y = -bullet->realVelocity.y;
		}
		
		if ((rand() % 4) < 3)
		{
			bullet->realVelocity.z = -bullet->realVelocity.z;
		}
		
		bullet->health = 200;
		
		addShieldHitParticles(bullet->position);
		audio->playSound(SND_ELECTRIC_SPARK4, CH_ANY, camera->getSoundDistance(bullet->position));
	}
}

void eyeDroidHuntCannon()
{
	Vector target;

	Boss *boss = (Boss*)self;
	 
	boss->target = (Boss*)entityManager->bossList.getFirstElement()->next;
	
	// cannon is dead
	if (boss->target == NULL)
	{
		boss->action = boss->walk;
		boss->thinkTime = 0;
		return;
	}
	
	if ((Math::getDistance(boss->position, boss->target->position) > 650) || (!unitCanSeeTarget(boss, boss->target, boss->position)))
	{
		target = self->position;
		
		if ((rand() % 4) == 0)
		{
			target.x += Math::rrand(-250, 250);
			target.y += Math::rrand(-250, 250);
			target.z += Math::rrand(-250, 250);
			boss->velocity = target;
			boss->velocity -= boss->position;
			boss->velocity.normalize();
			boss->velocity = boss->velocity * 1.5;
			faceLocation(boss, target);
		}
		else
		{
			boss->velocity = boss->target->position;
			boss->velocity.x += Math::rrand(-50, 50);
			boss->velocity.y += Math::rrand(-50, 50);
			boss->velocity.z += Math::rrand(50, 100);
			boss->velocity -= boss->position;
			boss->velocity.normalize();
			boss->velocity = boss->velocity * 2;
			faceLocation(boss, boss->target->position);
		}
			
		self->thinkTime = Math::rrand(50, 150);
		
		return;
	}
	
	boss->velocity = boss->velocity * 0.99;
	
	if (boss->getHealthPercent() > 25)
	{
		eyeDroidCommanderPlasmaAttack();
		boss->thinkTime = 0;
	}
	else
	{
		// fire rocket to destroy cannon
		eyeDroidCommanderRocketAttack();
		boss->thinkTime = 0;
	}
	
	boss->action = eyeDroidHuntCannon;
	
	if ((boss->target->health <= 1) || (boss->target->flags & EF_DEAD))
	{
		boss->action = boss->walk;
		boss->thinkTime = 0;
	}
}

void eyeDroidCommanderEnable()
{
	Boss *boss = (Boss*)self;
	
	self->flags |= EF_WEIGHTLESS;
	self->flags &= ~EF_BOUNCES;
	
	if (boss->getHealthPercent() > 25)
	{
		self->flags |= EF_IMMORTAL;
	}
	
	self->thinkTime = 75;
	self->action = eyeDroidHuntCannon;
	
	self->velocity.set(0, 0, 1);
	self->rotation.set(0, 0, 0);
	
	boss->shield = boss->maxShield;
}

void eyeDroidCommanderDisable()
{
	Boss *boss = (Boss*)self;
	
	self->flags &= ~EF_WEIGHTLESS;
	self->flags &= ~EF_IMMORTAL;
	self->flags |= EF_BOUNCES;
	
	self->thinkTime = 650;
	
	self->rotation.x = 25;
	self->rotation.y = -25;
	
	self->velocity.set(0, 0, 0);
	
	self->action = eyeDroidCommanderEnable;
	
	boss->shield = 0;
}

void eyeDroidCommanderDie1()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	self->rotation.x += engine->getTimeDifference(TD_LOGIC) * 3;
	self->rotation.y += engine->getTimeDifference(TD_LOGIC) * 3;
	self->rotation.z += engine->getTimeDifference(TD_LOGIC) * 3;
	
	if (self->custom <= 0)
	{
		self->action = eyeDroidCommanderDie2;
		self->custom = 250;
	}
}

void eyeDroidCommanderChase()
{
	Boss *boss = (Boss*)self;
	
	bossSightTarget(boss, 1000);
	
	if (boss->target == NULL)
	{
		return;
	}
	
	boss->velocity = boss->target->position;
	
	boss->velocity.x += Math::rrand(-50, 50);
	boss->velocity.y += Math::rrand(-50, 50);
	boss->velocity.z += Math::rrand(50, 100);
	
	boss->velocity -= boss->position;
	boss->velocity.normalize();

	float speed = Math::rrand(150, 400);
	speed /= Math::rrand(100, 250);
	
	boss->velocity.x *= speed;
	boss->velocity.y *= speed;
	boss->velocity.z *= speed;
	
	boss->thinkTime = Math::rrand(100, 200);
	
	faceLocation(boss, boss->target->position);
	
	boss->action = &eyeDroidCommanderSeek;
}

void eyeDroidCommanderSummonHelp()
{
	Vector position;
	String classname;
	Properties props;

	Boss *boss = (Boss*)self;
	
	bossSightTarget(boss, 1000);
	
	if (boss->target == NULL)
	{
		return;
	}
	
	self->velocity = self->velocity * 0.1;
	self->thinkTime = 75;
	
	for (int i = 0 ; i < self->custom ; i++)
	{
		position = self->position;
		
		switch (i)
		{
			case 0:
				position.x += 75;
				break;
				
			case 1:
				position.x -= 75;
				break;
				
			case 2:
				position.y += 75;
				break;
				
			case 3:
				position.y -= 75;
				break;
		}
		
		props.setProperty("classname", extractEnemyFromList("NME_PISTOL_DROID|NME_MACHINEGUN_DROID|NME_SHOTGUN_DROID"));
		props.setProperty("position", position);
		props.setProperty("thinkTime", Math::rrand(100, 200)); // don't attack instantly!
		addEnemy(&props);
		addTeleportParticles(position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(position));
	}
}

void eyeDroidCommanderSeek()
{
	Vector end;

	if (entityManager->enemyList.getSize() < 8)
	{
		float percent = self->getHealthPercent();
		
		int r = 0;
		if (percent < 25)
		{
			r = 5;
		}
		else if (percent < 50)
		{
			r = 10;
		}
		else
		{
			r = 15;
		}
		
		if ((rand() % r) == 0)
		{
			self->custom = Math::rrand(1, 4);
			eyeDroidCommanderSummonHelp();
			return;
		}
	}
	
	end = self->position;
	end.z -= 64;
	
	bsp->traceRay(self->position, end);
	
	if (tracer->outputFraction != 1)
	{
		self->velocity.z = 0.25;
		self->thinkTime = 50;
		return;
	}
	
	eyeDroidCommanderChase();
	
	if ((rand() % 10) <= 8)
	{
		self->action = &eyeDroidCommanderAttack;
		self->custom = Math::rrand(6, 12);
	}
}

void eyeDroidCommanderPlasmaAttack()
{
	Boss *boss = (Boss*)self;
	
	faceLocation(boss, boss->target->position);
	
	if (boss->reload > 0)
	{
		return;
	}
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_PLASMA_RIFLE], getGunPosition(boss, 0), boss->target->position);
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_PLASMA_RIFLE], getGunPosition(boss, 1), boss->target->position);
	
	audio->playSound(SND_HEAVY_PLASMA, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 15;
	
	if ((--self->custom) == 0)
	{
		self->action = eyeDroidCommanderSeek;
		self->thinkTime = 250;
	}
}

void eyeDroidCommanderRocketAttack()
{
	Boss *boss = (Boss*)self;
	
	faceLocation(boss, boss->target->position);
	
	if (boss->reload > 0)
	{
		return;
	}
	
	addRocket(boss, self->position, boss->target->position);
	audio->playSound(SND_ROCKET_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 200;
	
	self->action = eyeDroidCommanderSeek;
	self->thinkTime = 250;
}

void eyeDroidCommanderMineAttack()
{
	Vector velocity;

	Boss *boss = (Boss*)self;
	
	faceLocation(boss, boss->target->position);
	
	if (boss->reload > 0)
	{
		return;
	}
	
	velocity.x = Math::rrand(-25, 25);
	velocity.y = Math::rrand(-25, 25);
	velocity.z = -10;
	
	velocity = velocity * 0.01;
	
	addMiniMine(boss, boss->position, velocity);
	
	audio->playSound(SND_GRENADE_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = Math::rrand(35, 60);
	
	if ((--self->custom) == 0)
	{
		self->action = eyeDroidCommanderSeek;
		self->thinkTime = 250;
	}
}
		
void eyeDroidCommanderAttack()
{
	Boss *boss = (Boss*)self;
	
	bossSightTarget(boss, 1000);
	
	if (boss->target == NULL)
	{
		return;
	}
	
	if (!unitCanSeeTarget(boss, boss->target, boss->position))
	{
		boss->action = eyeDroidCommanderChase;
		return;
	}
	
	if (Math::getDistance(boss->position, boss->target->position) > 3250)
	{
		boss->action = eyeDroidCommanderChase;
		return;
	}
	
	int r = rand() % 10;
	
	self->velocity = self->velocity * 0.25;
	
	if (r == 0)
	{
		self->action = eyeDroidCommanderRocketAttack;
	}
	else if (r < 3)
	{
		self->action = eyeDroidCommanderMineAttack;
		self->custom = Math::rrand(4, 12);
	}
	else
	{
		self->action = eyeDroidCommanderPlasmaAttack;
	}
}

void eyeDroidCommanderInit(Boss *boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "Eye Droid Commander";
	
	boss->walk = &eyeDroidCommanderSeek;
	boss->attack = &eyeDroidCommanderSeek;
	boss->die = &eyeDroidCommanderSeek;
	boss->reactToBullet = &eyeDroidCommanderReactToBullet;
	boss->reactToDamage = &eyeDroidCommanderReactToDamage;
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
	
	boss->showHealthData = false;

	boss->thinkTime = 0;

	boss->action = entityDoNothing;
}

// ============================================ Ion Cannon =========================================

void ionCannonDie()
{
	// don't draw the base and the stand, etc...
	self->draw = &drawBoss;
	generalMechLargeExplosion();
	game->setInfoMessage(INFO_BAD, _("!!! ION CANNON DESTROYED !!!"));
}

void ionCannonRepair(Unit *engineer)
{
	float before = self->getHealthPercent();
	
	if ((self->owner != engineer) && (engineer != player))
	{
		game->setInfoMessage(INFO_HINT, _("%s starting repairs!"), engineer->getName());
		self->owner = engineer;
	}
	
	self->health += (self->maxHealth * 0.00075);
	self->thinkTime = 1;
	
	Math::limit(&self->health, 1, self->maxHealth);
	
	float after = self->getHealthPercent();
	
	for (int i = 20 ; i < 100 ; i += 20)
	{
		if ((before < i) && (after >= i))
		{
			game->setInfoMessage(INFO_HINT, _("Ion Cannon: %d%% Complete..."), i);
		}
	}
}

void ionCannonUseBlob()
{
	self->thinkTime = 100;
	
	if (Math::getDistance(self->position, player->position) <= 50)
	{
		if (game->isControl(CTRL_ACTION))
		{
			ionCannonRepair(player);
			game->setGamePlayMessage(INFO_GENERAL, _("Reparing Ion Cannon..."));
		}
		else
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Hold ACTION to repair cannon"));
		}
		
		self->thinkTime = 0;
	}
	
	Unit *engineer = (Unit*)entityManager->blobList.getTail();
	
	if (self->health == self->maxHealth)
	{
		self->action = self->walk;
		self->thinkTime = 0;
		self->custom = 750;
		game->setInfoMessage(INFO_HINT, _("Ion Cannon Repaired!"));
		
		engineer->action = engineer->walk;
		
		Switch *swt;
		
		for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
		{
			if (entity->entityType != ET_SWITCH)
			{
				continue;
			}
			
			swt = (Switch*)entity;
			swt->action = switchLookForPlayer;
			swt->active = 0;
			swt->thinkTime = 0;
		}
		
		self->owner = NULL;
	}
	
	if (engineer->health <= 0)
	{
		self->thinkTime = 250;
		return;
	}
	
	if (engineer->definition->type == BLOB_TEEKA)
	{
		return;
	}
	
	if (Math::getDistance(self->position, engineer->position) <= 25)
	{
		engineer->velocity.set(0, 0, 0);
		engineer->thinkTime = 100;
		ionCannonRepair(engineer);
		return;
	}
	
	// tell our guy to come over and repair us!
	
	float z = engineer->velocity.z;
		
	faceLocation(engineer, self->position);
	
	engineer->velocity = self->position;
	engineer->velocity -= engineer->position;
	engineer->velocity.normalize();
	
	float speed = 1.0;
	
	engineer->velocity.x /= speed;
	engineer->velocity.y /= speed;
	engineer->velocity.z = z;
	
	engineer->thinkTime = 100;
}

void ionCannonReactToDamage(Entity *owner, float damage)
{
	float percent = ((Boss*)entityManager->bossList.getFirstElement())->getHealthPercent();
	
	if ((owner == entityManager->bossList.getFirstElement()) && (percent <= 25))
	{
		self->action = self->die;
		self->thinkTime = 0;
		self->custom = 0;
		return;
	}
}

void ionCannonReactToBullet(Bullet *bullet)
{
	if (self->action == &ionCannonUseBlob)
	{
		return;
	}
	
	if (bullet != NULL)
	{
		bullet->health = 0;
		bullet->touch = NULL;
		addSparkParticle(bullet->position);
		audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
		
		Boss *boss = (Boss*)self;
		
		if (bullet->definition->type != WEP_SHOTGUN)
		{
			boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
		}
		else
		{
			boss->applyDamage(1);
		}
	}
	
	float percent = ((Boss*)entityManager->bossList.getFirstElement())->getHealthPercent();
	
	if ((bullet->owner == entityManager->bossList.getFirstElement()) && (percent <= 25))
	{
		self->action = self->die;
		self->thinkTime = 0;
		self->custom = 0;
		return;
	}
	
	Math::limit(&self->health, 1, self->maxHealth);
		
	if (self->health == 1)
	{
		doExplosion(self->position, 25, self);
		
		game->setInfoMessage(INFO_BAD, _("Ion Cannon Damaged!"));
		self->action = ionCannonUseBlob;
		self->thinkTime = 100;
	}
}

void ionCannonTrack()
{
	Vector v, p;

	Boss *boss = (Boss*)self;
	
	boss->target = (Entity*)entityManager->bossList.getFirstElement();
	
	faceLocation(boss, boss->target->position);
	
	boss->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (boss->custom <= 0)
	{
		if (!unitCanSeeTarget(boss, boss->target, boss->position))
		{
			boss->custom = 250;
			return;
		}
		
		game->setInfoMessage(INFO_HINT, _("Droid Commander Disabled!!"));
		
		boss->target->action = eyeDroidCommanderDisable;
		boss->target->thinkTime = 0;
		boss->action = entityDoNothing;
		
		v = boss->target->position;
		v -= boss->position;
		v.normalize();
		
		p = boss->position;
		Particle *particle;
		
		int distance = (int)(Math::getDistance(boss->position, boss->target->position));
		
		for (int i = 0 ; i < distance ; i += 5)
		{
			p += (v * 5);
			
			particle = particleManager->spawnParticle();
			particle->position = p;
			particle->life = Math::rrand(100, 150);
			particle->size = 1;
			particle->color = ((i % 2) == 0) ? GLColor::purple : GLColor::white;
		}
		
		addShieldDestroyedParticles(boss->target->position);
		addShieldDestroyedParticles(boss->target->position);
		
		audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(boss->position));
		audio->playSound(SND_GRENADE_EXPLODE, CH_ANY, camera->getSoundDistance(boss->target->position));
	}
}

void ionCannonEnable()
{
	Boss *boss = (Boss*)entityManager->bossList.getFirstElement()->next;
	
	boss->action = ionCannonTrack;
	boss->thinkTime = 0;
	boss->custom = 300;
	
	game->setInfoMessage(INFO_HINT, _("Ion Cannon Enabled"));
}

// ================= Special Trigger for enabling the bosses ============

void bossBattle1Start()
{
	audio->stopMusic();
	audio->loadMusic("music/Super Robot Fighter.xm");
	audio->playMusic();
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		boss->showHealthData = true;
	}
	
	Boss *commander = (Boss*)entityManager->bossList.getFirstElement();
	Boss *cannon = (Boss*)entityManager->bossList.getFirstElement()->next;
	
	commander->action = commander->walk;
	commander->thinkTime = 0;
	commander->velocity = cannon->position;
	commander->velocity -= commander->position;
	commander->velocity.normalize();
	commander->velocity = commander->velocity * 1.5;
	commander->velocity.z = 0;
	faceLocation(commander, cannon->position);
	
	engine->resetTimeDifference();
}

void ionCannonInit(Boss *boss)
{
	BossDef *bossDef = &entityManager->bossDef[1];
	
	boss->name = "Ion Cannon";
	boss->entityType = ET_NEUTRAL;
	
	boss->walk = &entityDoNothing;
	boss->attack = &entityDoNothing;
	boss->die = &ionCannonDie;
	boss->reactToBullet = &ionCannonReactToBullet;
	boss->reactToDamage = &ionCannonReactToDamage;
	boss->draw = &drawSentryGun;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	boss->gunPosition[0] = bossDef->gunPosition[0];
	boss->gunPosition[1] = bossDef->gunPosition[1];
	
	boss->health = boss->maxHealth = bossDef->health;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->showHealthData = false;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = boss->walk;
	
	// Set up the Boss Trigger
	
	Trigger *trigger = (Trigger*)mission->triggerList.getFirstElement();
	trigger->specialAction = ionCannonEnable;
	trigger = (Trigger*)trigger->next;
	trigger->specialAction = bossBattle1Start;
}
