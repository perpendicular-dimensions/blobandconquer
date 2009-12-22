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

#include "spiderBlobCommander.h"
		
void spiderBlobCommanderSearch();
void spiderBlobCommanderStand();

void spiderBlobCommanderDie()
{
	generalMechLargeExplosion();
		
	// kill all enemies
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		unit->health = 0;
	}
	
	Mix_FadeOutMusic(5000);
	
	handleStructureStateChange("ExitDoor", true);
	handleStructureStateChange("lavaPit", false);
	
	self->flags |= EF_DEAD;
}

void spiderBlobCommanderDestroySwitch()
{
	Switch *swt = NULL;
	Switch *closest = NULL;
	float distance = 0;
	float currentDistance = RAND_MAX;
			
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType == ET_SWITCH)
		{
			swt = (Switch*)entity;
			swt->active = false;
			swt->action = &switchLookForPlayer;
			swt->thinkTime = 100;
			
			distance = Math::getDistance(swt->position, self->position);
			
			if (distance < currentDistance)
			{
				closest = swt;
				currentDistance = distance;
			}
		}
	}
	
	// shouldn't happen to be honest...
	if (closest != NULL)
	{
		closest->action = NULL;
		closest->health = 0;
		closest->flags |= EF_DEAD;
		doExplosion(closest->position, 25, closest);
	}
}

void spiderBlobCommanderBurn()
{
	// keep doors open
	handleStructureStateChange("lavaPit", true);
	
	self->health -= (engine->getTimeDifference(TD_LOGIC) * 2.5);
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if ((rand() % 10) == 0)
	{
		Vector v = self->position;
		v.x += Math::rrand(-15, 15);
		v.y += Math::rrand(-15, 15);
		v.z += Math::rrand(-15, 15);
		audio->playSound(SND_SPIDERBLOB_DIE, CH_ANY, camera->getSoundDistance(self->position));
		doExplosion(v, Math::rrand(5, 10), self);
	}
	
	if (self->custom <= 0)
	{
		if (self->health > 0)
		{
			spiderBlobCommanderDestroySwitch();
			spiderBlobCommanderStand();
		}
		else
		{
			self->action = self->die;
			self->thinkTime = 0;
		}
	}
}

void spiderBlobCommanderCollapse()
{
	self->flags &= ~EF_GRAPPLING;
	self->flags &= ~EF_WEIGHTLESS;
	self->velocity.x = self->velocity.y = 0;
	if (self->velocity.z > 0)
	{
		self->velocity.z = 0;
	}
	
	//self->currentAnim = NULL;
	
	if (!(self->contentType & CONTENTS_SOLID))
	{
		self->thinkTime = 0;
		self->custom = 400;
		self->walk = spiderBlobCommanderBurn;
		self->action = spiderBlobCommanderBurn;
		audio->playSound(SND_SPIDERBLOB_DIE, CH_ANY, camera->getSoundDistance(self->position));
		return;
	}
	
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		self->walk = spiderBlobCommanderStand;
		self->action = spiderBlobCommanderStand;
	}
}

void spiderBlobCommanderSummon()
{
	String target;
	Vector position;
	Properties props;
	
	Boss *boss = (Boss*)self;
	if (!boss->showHealthData)
	{
		return;
	}
	
	target.setText("babySpiderPoint%d", Math::rrand(1, 8));
	position = entityManager->getEntityPosition(target.getText());
	
	props.setProperty("classname", extractEnemyFromList("NME_BABY_SPIDERBLOB"));
	props.setProperty("position", position);
	props.setProperty("thinkTime", 0); // don't attack instantly!
	addEnemy(&props);
}
		
void spiderBlobCommanderEnable()
{
	audio->stopMusic();
	audio->loadMusic("music/Super Robot Fighter.xm");
	audio->playMusic();
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		boss->showHealthData = true;
	}
	
	engine->resetTimeDifference();
}
		
void spiderBlobCommanderAttack()
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
		self->action = spiderBlobCommanderSearch;
		
		if (boss->shield > 0)
		{
			self->thinkTime = 250;
		}
	}
}

void spiderBlobCommanderClusterAttack()
{
	self->velocity.x = self->velocity.y = 0;
	
	Vector targetPosition = self->position;
	targetPosition.x += Math::rrand(-300, 300);
	targetPosition.y += Math::rrand(-300, 300);
	addLauncherShot(self, &entityManager->weaponDef[WEP_CLUSTER_LAUNCHER], self->position, targetPosition);
	
	audio->playSound(SND_GRENADE_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	if (--self->custom <= 0)
	{
		self->action = self->walk;
	}
	
	self->thinkTime = 50;
}

void spiderBlobCommanderReactToDamage(Entity *owner, float damage)
{
	// already fallen over
	if ((self->walk == spiderBlobCommanderCollapse) || (damage < 60))
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	if ((boss->shield <= 0) && (owner == player))
	{
		getAnimation("alt_die", boss);
		
		self->flags |= EF_NO_MD2_LOOP;
		
		self->boundingBox.mins.x = -8;
		self->boundingBox.maxs.x = 8;
		self->boundingBox.mins.z = -2;
		self->boundingBox.maxs.z = 2;
		self->updateBoundingBox();
		
		self->walk = spiderBlobCommanderCollapse;
		self->action = self->walk;
		self->thinkTime = 0;
		self->custom = 500;
	}
}

void spiderBlobCommanderReactToBullet(Bullet *bullet)
{
	Boss *boss = (Boss*)self;
	
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
	
	if (boss->shield <= 0)
	{
		return;
	}
	
	if (bullet->definition->type != WEP_SHOTGUN)
	{
		boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
	}
	else
	{
		boss->applyDamage(1);
	}
	
	if (boss->shield <= 0)
	{
		Vector v;
		
		for (int i = 0 ; i < 5 ; i++)
		{
			v = self->position;
			v.x += Math::rrand(-25, 25);
			v.y += Math::rrand(-25, 25);
			v.z += Math::rrand(-25, 25);
			doExplosion(v, 25, self);
		}
	}
}

void spiderBlobCommanderChase()
{
	Boss *boss = (Boss*)self;
	
	if (boss->shield > 0)
	{
		bossSightTarget(boss, 1000);
	}
	else
	{
		boss->target = player;
	}
	
	if (boss->target == NULL)
	{
		boss->currentAnim = NULL;
		boss->velocity.x = boss->velocity.y = 0;
		return;
	}
	
	float z = boss->velocity.z;
	
	boss->velocity = boss->target->position;
	boss->velocity.x += Math::rrand(-50, 50);
	boss->velocity.y += Math::rrand(-50, 50);
	boss->velocity -= boss->position;
	boss->velocity.normalize();

	boss->velocity.x *= 0.5;
	boss->velocity.y *= 0.5;
	
	boss->velocity.z = z;
	
	boss->thinkTime = Math::rrand(100, 200);
	
	if (boss->shield <= 0)
	{
		boss->thinkTime = 50;
	}
	
	faceLocation(boss, boss->target->position);
	
	boss->action = &spiderBlobCommanderSearch;
	
	getAnimation("stand", boss);
	
	self->flags &= ~EF_NO_MD2_LOOP;
	
	if (boss->flags & EF_ONGROUND)
	{
		if ((rand() % 10) == 0)
		{
			boss->velocity.x = boss->velocity.y = 0;
			boss->currentAnim = NULL;
			
			Vector hookPoint = boss->target->position;
			hookPoint.z += 200;
			hookPoint = bsp->getTraceLocation(boss->target->position, hookPoint);
			
			debug(("Spider Boss grappling to '%s'\n", hookPoint.toString()));
			
			addGrapplingHook(boss, hookPoint);
			audio->playSound(SND_GRAPPLE_FIRE, CH_PLAYER, camera->getSoundDistance(self->position));
			addLauncherShot(self, &entityManager->weaponDef[WEP_CLUSTER_LAUNCHER], self->position, boss->target->position);
		}
	}
}

void spiderBlobCommanderSelectAttack()
{
	Boss *boss = (Boss*)self;
	
	if ((rand() % 10) != 0)
	{
		boss->action = spiderBlobCommanderAttack;
		boss->custom = Math::rrand(8, 14);
		
		if (boss->shield > 0)
		{
			if ((rand() % 5) != 0)
			{
				boss->velocity.x = boss->velocity.y = 0;
				boss->currentAnim = NULL;
			}
		}
	}
	else
	{
		boss->action = spiderBlobCommanderClusterAttack;
		boss->custom = Math::rrand(2, 5);
	}
}

void spiderBlobPostGrapple()
{
	if (!(self->flags & EF_GRAPPLING))
	{
		if (self->custom == 50)
		{
			self->flags |= EF_WEIGHTLESS;
			handleStructureStateChange("lavaPit", false);
			
			Switch *swt;
			
			for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
			{
				if (entity->entityType == ET_SWITCH)
				{
					swt = (Switch*)entity;
					swt->active = false;
					swt->action = &switchLookForPlayer;
					swt->thinkTime = 100;
				}
			}
		}
		
		self->custom -= engine->getTimeDifference(TD_LOGIC);
		
		if (self->custom <= 0)
		{
			Boss *boss = (Boss*)self;
		
			self->flags &= ~EF_WEIGHTLESS;
			self->walk = spiderBlobCommanderSearch;
			self->action = spiderBlobCommanderSearch;
			
			self->scale = 3.5;
			self->boundingBox.mins = self->definition->bbMins;
			self->boundingBox.maxs = self->definition->bbMaxs;
			self->boundingBox.mins.x *= self->scale;
			self->boundingBox.mins.y *= self->scale;
			self->boundingBox.mins.z *= self->scale;
			self->boundingBox.maxs.x *= self->scale;
			self->boundingBox.maxs.y *= self->scale;
			self->boundingBox.maxs.z *= self->scale;
			self->updateBoundingBox();
			
			boss->flags &= ~EF_NO_MD2_LOOP;
			getAnimation("stand", boss);
			boss->shield = boss->maxShield;
		}
	}
}

void spiderBlobCommanderStand2()
{
	if (self->flags & EF_WEIGHTLESS)
	{
		self->flags |= EF_NO_MD2_LOOP;
		getAnimation("get_up", self);
		self->action = &entityDoNothing;
	}
}

void spiderBlobCommanderStand()
{
	Boss *boss = (Boss*)self;

	boss->velocity.x = boss->velocity.y = 0;
	boss->currentAnim = NULL;
	Vector hookPoint = boss->position;
	hookPoint.z += 99999;
	hookPoint = bsp->getTraceLocation(boss->position, hookPoint);
	
	Bullet *bullet;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		hookPoint.x += Math::rrand(-5, 5);
		hookPoint.y += Math::rrand(-5, 5);
		bullet = addGrapplingHook(boss, hookPoint);
		bullet->velocity = bullet->velocity * 0.5;
	}
	
	audio->playSound(SND_GRAPPLE_FIRE, CH_PLAYER, camera->getSoundDistance(self->position));
	boss->action = spiderBlobCommanderStand2;
	boss->walk = spiderBlobPostGrapple;
	boss->custom = 50;
	
	// shrink bounding box to ensure the Spider can get out...
	boss->boundingBox.mins.x = -1;
	boss->boundingBox.mins.y = -1;
	boss->boundingBox.maxs.x = 1;
	boss->boundingBox.maxs.y = 1;
	boss->updateBoundingBox();
	
	debug(("Spider Boss grappling to '%s'\n", hookPoint.toString()));
}

void spiderBlobCommanderSearch()
{
	Boss *boss = (Boss*)self;
	
	if (entityManager->enemyList.getSize() < 15)
	{
		int number = (entityManager->enemyList.getSize() / 2) + 1;
		
		if ((rand() % number) == 0)
		{
			spiderBlobCommanderSummon();
		}
	}
	
	if (!(boss->contentType & CONTENTS_SOLID))
	{
		if (!(boss->flags & EF_GRAPPLING))
		{
			spiderBlobCommanderStand();
			return;
		}
	}
	
	boss->action = spiderBlobCommanderChase;
	
	if ((boss->target != NULL) && ((rand() % 4) == 0))
	{
		self->action = spiderBlobCommanderSelectAttack;
	}
}

void spiderBlobCommanderInit(Boss *boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "Spider Blob Commander";
	boss->entityType = ET_BOSS;
	
	boss->walk = &spiderBlobCommanderSearch;
	boss->attack = &spiderBlobCommanderSearch;
	boss->die = &spiderBlobCommanderDie;
	boss->reactToBullet = &spiderBlobCommanderReactToBullet;
	boss->reactToDamage = &spiderBlobCommanderReactToDamage;
	boss->draw = &drawBoss;
	
	boss->scale = 3.5;
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->boundingBox.mins.x *= boss->scale;
	boss->boundingBox.mins.y *= boss->scale;
	boss->boundingBox.mins.z *= boss->scale;
	boss->boundingBox.maxs.x *= boss->scale;
	boss->boundingBox.maxs.y *= boss->scale;
	boss->boundingBox.maxs.z *= boss->scale;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	boss->gunPosition[0] = bossDef->gunPosition[0];
	boss->gunPosition[1] = bossDef->gunPosition[1];
	
	boss->health = boss->maxHealth = bossDef->health;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->showHealthData = false;

	boss->action = boss->walk;
	
	// Set up the Boss Trigger
	Trigger *trigger = (Trigger*)mission->triggerList.getFirstElement();
	if (trigger != NULL)
	{
		trigger->specialAction = spiderBlobCommanderEnable;
	}
}
