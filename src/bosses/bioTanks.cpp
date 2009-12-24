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

#include "bioTanks.h"

void drawBioTank(bool transparent)
{
	if (transparent)
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	glColor3f(1.0, 1.0, 1.0);
	
	glPushMatrix();
	{
		glTranslatef(boss->position.x, boss->position.y, boss->position.z);
		glScalef(boss->scale, boss->scale, boss->scale);
		glRotatef(boss->rotation.x, 0.0, 0.0, 1.0);
		glRotatef(boss->rotation.y, 0.0, 1.0, 0.0);
		glBindTexture(GL_TEXTURE_2D, boss->definition->texture[0]->data);
		glCallList(boss->definition->model[0]->data);
	}
	glPopMatrix();
	
	if (boss->health <= 0)
	{
		return;
	}
	
	glPushMatrix();
	{
		glTranslatef(boss->position.x, boss->position.y, boss->position.z + (8 * boss->scale));
		glScalef(boss->scale, boss->scale, boss->scale);
		glRotatef(boss->cannonRotation.x, 0.0, 0.0, 1.0);
		glRotatef(boss->cannonRotation.y, 0.0, 1.0, 0.0);
		glRotatef(boss->cannonRotation.z, 1.0, 0.0, 0.0);
		glBindTexture(GL_TEXTURE_2D, boss->definition->texture[1]->data);
		glCallList(boss->definition->model[1]->data);
	}
	glPopMatrix();
}

void bossBattle2Start()
{
	audio->stopMusic();
	audio->loadMusic("music/Super Robot Fighter.xm");
	audio->playMusic();
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		boss->showHealthData = true;
	}
	
	engine->resetTimeDifference();
	
	enableObjective(1);
}

void bioTankDoNothing()
{
	self->thinkTime = RAND_MAX;
}

void bioTankRunOver(Entity *other)
{
	if (other == NULL)
	{
		// hit a wall. Stop moving...
		if (tracer->outputNormal->z != 1.0)
		{
			self->action = self->walk;
			self->velocity.set(0, 0, 0);
			self->thinkTime = 50;
			self->touch = NULL;
		}
		return;
	}
	
	Unit *unit;
	
	switch (other->entityType)
	{
		case ET_BLOB:
			unit = (Unit*)other;
			if (unit->helpless == 0)
			{
				unit->health -= (5 * max(self->velocity.x, self->velocity.y));
				unit->helpless = 200;
				unit->velocity = self->velocity * 2;
				unit->velocity.z = 1.25;
				unit->flags |= EF_BOUNCES;
				audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(unit->position));
			}
			break;
			
		case ET_BIOMECH:
			unit = (Unit*)other;
			if (unit->health != 0)
			{
				audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(unit->position));
			}
			unit->health = 0;
			unit->velocity = self->velocity * 2;
			unit->velocity.z = 1.25;
			break;
			
		case ET_BOSS:
			self->action = self->walk;
			self->velocity.set(0, 0, 0);
			self->thinkTime = 0;
			break;
			
		case ET_SUPPLY_CRATE:
			((Structure*)other)->health = 0;
			break;
			
		default:
			break;
	}
}

void bioTankForward()
{
	float dir = toRadians(self->rotation.x);
	
	float speed = Math::rrand(15, 25);
	speed *= 0.1;
	
	self->velocity.x = (speed * -sin(dir));
	self->velocity.y = (speed * cos(dir));
	
	self->thinkTime = 150;
	self->action = self->walk;
}

void bioTankBackward()
{
	float dir = toRadians(self->rotation.x);
	
	float speed = Math::rrand(15, 25);
	speed *= 0.1;
	
	self->velocity.x = -(speed * -sin(dir));
	self->velocity.y = -(speed * cos(dir));
	
	self->thinkTime = 150;
	self->action = self->walk;
}

void bioTankRotate()
{
	if (self->custom > 0)
	{
		self->rotation.x -= 0.5 * engine->getTimeDifference(TD_LOGIC);
		self->custom -= engine->getTimeDifference(TD_LOGIC);
		
		if (self->custom <= 0)
		{
			self->action = self->walk;
		}
	}
	else
	{
		self->rotation.x += 0.5 * engine->getTimeDifference(TD_LOGIC);
		self->custom += engine->getTimeDifference(TD_LOGIC);
		
		if (self->custom >= 0)
		{
			self->action = self->walk;
		}
	}
}

void bioTankChaseTarget()
{
	Boss *boss = (Boss*)self;
	
	if (boss->target == NULL)
	{
		return;
	}
	
	boss->velocity = boss->target->position;
	
	boss->velocity.x += Math::rrand(-50, 50);
	boss->velocity.y += Math::rrand(-50, 50);
	
	boss->velocity -= boss->position;
	boss->velocity.normalize();
	
	float speed = Math::rrand(15, 25);
	speed *= 0.1;
	
	boss->velocity.x *= speed;
	boss->velocity.y *= speed;
	boss->velocity.z = 0;
	
	boss->thinkTime = Math::rrand(50, 100);
	boss->touch = bioTankRunOver;
	
	faceLocation(boss, boss->target->position);
}

void bioTankSeekBeta();
void bioTankSeek();

void bioTankWaitForHack()
{
	Boss *boss = (Boss*)self;
	
	if (Math::getDistance(player->position, boss->position) <= 50)
	{
		int slot = game->getItemSlot(_("AI Kernel Module"));
		
		if ((slot == -1) && (!game->cheatItems))
		{
			game->setGamePlayMessage(INFO_GENERAL, _("AI Kernel Module required"));
			return;
		}
		
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to apply AI Kernel Module"));
		
		if (game->isControl(CTRL_ACTION))
		{
			if (!game->cheatItems)
			{
				game->item[slot]->itemPower--;
				
				if (game->item[slot]->itemPower == 0)
				{
					game->removeItem(_("AI Kernel Module"));
				}
			}
			
			game->resetControl(CTRL_ACTION);
			boss->walk = bioTankSeekBeta;
			boss->maxHealth *= 1.5;
			boss->action = boss->walk;
			boss->shield = boss->maxShield;
			boss->health = boss->maxHealth;
			boss->flags |= EF_HAS_TURRET;
			boss->thinkTime = 300;
			char name[256];
			strcpy(name, boss->name.getText());
			boss->name.setText(_("%s (Patched)"), name);
			audio->playSound(SND_SWITCH_BLEEP, CH_ANY, camera->getSoundDistance(boss->position));
			
			if (player->target == boss)
			{
				player->target = NULL;
			}
			
			return;
		}
	}
	
	if (boss->custom > 0)
	{
		boss->custom -= engine->getTimeDifference(TD_LOGIC);
		return;
	}
	
	boss->health += (0.05 * engine->getTimeDifference(TD_LOGIC));
	
	Math::limit(&boss->health, 0, boss->maxHealth);
	
	if (boss->health == boss->maxHealth)
	{
		// start working again...
		boss->thinkTime = 0;
		boss->action = boss->walk;
		boss->entityType = ET_BOSS;
		boss->flags |= EF_HAS_TURRET;
		game->setInfoMessage(INFO_HINT, _("%s has recovered!"), boss->getName());
	}
}

void bioTankSeekBeta()
{
	Boss *boss = (Boss*)self;
	 
	boss->target = (Boss*)entityManager->bossList.getFirstElement();
	
	// sit there and do nothing...
	if (boss->target->name != "BioMech Tank V3.0 (Beta)")
	{
		boss->thinkTime = RAND_MAX;
		boss->velocity.x = boss->velocity.y = 0; 
		return;
	}
	
	if (!unitCanSeeTarget(boss, boss->target, boss->position))
	{
		boss->velocity = boss->target->position;
		boss->velocity.x += Math::rrand(-50, 50);
		boss->velocity.y += Math::rrand(-50, 50);
		boss->velocity.z = 0;
		boss->velocity -= boss->position;
		boss->velocity.normalize();
		boss->velocity = boss->velocity * 2;
		boss->thinkTime = 100;
		faceLocation(boss, boss->target->position);
		return;
	}
	
	if (Math::getDistance(boss->position, boss->target->position) > 350)
	{
		boss->thinkTime = 50;
		bioTankChaseTarget();
		return;
	}
	
	boss->velocity.set(0, 0, 0);
	boss->action = boss->attack;
	boss->thinkTime = 0;
}

void bioTankSeek()
{
	Boss *boss = (Boss*)self;
	
	boss->touch = NULL;
	boss->velocity.x = self->velocity.y = 0;
	
	if (!boss->showHealthData)
	{
		return;
	}
	
	int r = Math::rrand(0, 8);
	
	if (r == 1)
	{
		boss->touch = bioTankRunOver;
		boss->action = bioTankForward;
	}
	else if (r == 2)
	{
		boss->touch = bioTankRunOver;
		boss->action = bioTankBackward;
	}
	else if (r == 3)
	{
		boss->custom = Math::rrand(-200, 200);
		boss->action = bioTankRotate;
	}
	else
	{
		boss->target = NULL;
		bioTankChaseTarget();
		boss->action = boss->attack;
	}
}

void bioTankDie2()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		generalMechLargeExplosion();
	}
}

void bioTankTopInit(Unit *unit, BossDef *bossDef);

void bioTankDie1()
{
	self->custom = Math::rrand(250, 450);
	self->action = bioTankDie2;
	self->walk = bioTankDie2;
	
	BossDef *tankTop;
	
	if (self->definition == &entityManager->bossDef[1])
	{
		tankTop = &entityManager->bossDef[4];
	}
	else
	{
		tankTop = &entityManager->bossDef[5];
	}
	
	if (!(self->flags & EF_DYING))
	{
		Unit *unit = entityManager->spawnEnemy(NME_DEAD_SENTRYGUN, 0);
		bioTankTopInit(unit, tankTop);
		self->flags |= EF_DYING;
	}
}

void bioTankReactToDamage(Entity *owner, float amount)
{
	Boss *boss = (Boss*)self;
	
	boss->applyDamage(amount);
	
	if ((boss->getHealthPercent() <= 50) && (boss->entityType != ET_NEUTRAL))
	{
		boss->thinkTime = 0;
		boss->action = bioTankWaitForHack;
		boss->custom = 400;
		boss->touch = NULL;
		boss->entityType = ET_NEUTRAL;
		boss->flags &= ~EF_HAS_TURRET;
		boss->velocity.x = boss->velocity.y = 0;
		doExplosion(self->position, 25, self);
		game->setInfoMessage(INFO_HINT, _("%s has been disabled!"), boss->getName());
		return;
	}
	
	if (boss->health <= 0)
	{
		self->action = bioTankDie1;
		self->walk = bioTankDie1;
		self->thinkTime = 0;
	}
}

void bioTankReactToBullet(Bullet *bullet)
{
	Boss *boss = (Boss*)self;
	
	addSparkParticle(bullet->position);
	audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
	addSparkParticle(bullet->position);
	bullet->health = 0;
	
	if (bullet->definition->type != WEP_SHOTGUN)
	{
		boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
	}
	else
	{
		boss->applyDamage(1);
	}
	
	if ((boss->getHealthPercent() <= 50) && (boss->entityType != ET_NEUTRAL))
	{
		boss->thinkTime = 0;
		boss->action = bioTankWaitForHack;
		boss->custom = 400;
		boss->touch = NULL;
		boss->entityType = ET_NEUTRAL;
		boss->velocity.x = boss->velocity.y = 0;
		doExplosion(self->position, 25, self);
		game->setInfoMessage(INFO_HINT, _("%s has been disabled!"), boss->getName());
		return;
	}
	
	if (boss->health <= 0)
	{
		self->action = bioTankDie1;
		self->thinkTime = 0;
	}
}

void bioTankRocketAttack()
{
	Vector targetPosition;
	
	Boss *boss = (Boss*)self;

	boss->velocity = boss->velocity * 0.9995;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	targetPosition = boss->target->position;
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_ROCKET_LAUNCHER], getGunPosition(boss, 0), targetPosition);
	
	audio->playSound(SND_ROCKET_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 50;
	
	if ((--self->custom) == 0)
	{
		self->action = self->walk;
		self->thinkTime = 250;
	}
}

void bioTankMachineGunAttack()
{
	Boss *boss = (Boss*)self;
	
	boss->velocity = boss->velocity * 0.9995;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_UZI], getGunPosition(boss, 0), boss->target->position);
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_UZI], getGunPosition(boss, 1), boss->target->position);
	
	audio->playSound(SND_UZI, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 15;
	
	if ((--self->custom) == 0)
	{
		self->action = self->walk;
		self->thinkTime = 250;
	}
}

void bioTankMortorAttack()
{
	Vector targetPosition;

	Boss *boss = (Boss*)self;
	
	boss->velocity = boss->velocity * 0.9995;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	targetPosition = boss->target->position;
	targetPosition.x += Math::rrand(-25, 25);
	targetPosition.y += Math::rrand(-25, 25);
	targetPosition.z = boss->position.z;
	addLauncherShot(self, &entityManager->weaponDef[WEP_GRENADE_LAUNCHER], getGunPosition(boss, 0), targetPosition);
	
	audio->playSound(SND_GRENADE_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 25;
	
	if ((--self->custom) == 0)
	{
		self->action = self->walk;
		self->thinkTime = 250;
	}
}

// ======================= Biotank #1 ============================

void bioTank1Attack()
{
	Boss *boss = (Boss*)self;
	
	boss->velocity.x = boss->velocity.y = 0;
	
	float distance;
	
	if ((boss->target != NULL) && (boss->target->health > 0))
	{
		distance = Math::getDistance(boss->position, boss->target->position);
	}
	else
	{
		distance = bossSightTarget(boss, 2500);
		
		if (boss->target == NULL)
		{
			self->action = self->walk;
			return;
		}
	}
	
	if (distance > 650)
	{
		bioTankChaseTarget();
		return;
	}
	
	faceLocation(boss, boss->target->position);
	
	if (boss->target->entityType != ET_BOSS)
	{
		if ((rand() % 5) == 0)
		{
			boss->custom = Math::rrand(1, 2);
			boss->action = bioTankRocketAttack;
		}
		else
		{
			boss->custom = Math::rrand(10, 15);
			boss->action = bioTankMachineGunAttack;
		}
	}
	else
	{
		boss->custom = Math::rrand(2, 4);
		boss->action = bioTankRocketAttack;
	}
}

void bioTank1Init(Boss* boss)
{
	BossDef *bossDef = &entityManager->bossDef[1];
	
	boss->name = "BioMech Tank V1.9";
	
	boss->walk = &bioTankSeek;
	boss->attack = &bioTank1Attack;
	boss->die = &bioTankSeek;
	boss->reactToBullet = &bioTankReactToBullet;
	boss->reactToDamage = &bioTankReactToDamage;
	boss->draw = &drawBioTank;
	
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
}

// ==================== Bio Tank #2 =======================================

void bioTank2Attack()
{
	Boss *boss = (Boss*)self;
	
	boss->velocity.x = boss->velocity.y = 0;
	
	float distance;
	
	if ((boss->target != NULL) && (boss->target->health > 0))
	{
		distance = Math::getDistance(boss->position, boss->target->position);
	}
	else
	{
		distance = bossSightTarget(boss, 2500);
		
		if (boss->target == NULL)
		{
			self->action = self->walk;
			return;
		}
	}
	
	if (distance > 650)
	{
		bioTankChaseTarget();
		return;
	}
	
	if ((rand() % 5) != 0)
	{
		bioTankChaseTarget();
	}
	else
	{
		faceLocation(boss, boss->target->position);
	}
	
	if (boss->target->entityType != ET_BOSS)
	{
		if ((rand() % 5) == 0)
		{
			boss->custom = Math::rrand(3, 5);
			boss->action = bioTankMortorAttack;
			boss->velocity.set(0, 0, 0);
		}
		else
		{
			boss->custom = Math::rrand(10, 15);
			boss->action = bioTankMachineGunAttack;
		}
	}
	else
	{
		boss->custom = Math::rrand(3, 5);
		boss->action = bioTankMortorAttack;
		boss->velocity.set(0, 0, 0);
	}
}

void bioTank2Init(Boss* boss)
{
	BossDef *bossDef = &entityManager->bossDef[2];
	
	boss->name = "BioMech Tank V2.5";
	
	boss->walk = &bioTankSeek;
	boss->attack = &bioTank2Attack;
	boss->die = &bioTankSeek;
	boss->reactToBullet = &bioTankReactToBullet;
	boss->reactToDamage = &bioTankReactToDamage;
	boss->draw = &drawBioTank;
	
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
}

// =================== BioTank 3 ======================================

void bioTank3SpawnAIHacks()
{
	Vector v;
	int count = 0;
	
	for (Item *item = (Item*)entityManager->itemList.getFirstElement() ; item != NULL ; item = (Item*)item->next)
	{
		if (item->definition->type < ITM_CUSTOM_FIRST)
		{
			continue;
		}
		
		count++;
	}
	
	if (count > 2)
	{
		return;
	}
	
	Properties props;
	props.name = "ITM_CUSTOM";
	props.setProperty("name", _("AI Kernel Module"));
	props.setProperty("definitionName", _("AI Kernel Module"));
	
	switch (rand() % 4)
	{
		case 0:
			v.set(-720, 1608, 72);
			break;
			
		case 1:
			v.set(-896, 296, 72);
			break;
			
		case 2:
			v.set(1176, 808, 72);
			break;
			
		case 3:
			v.set(1056, 1128, 72);
			break;
	}
	
	v.x += Math::rrand(-15, 15);
	v.y += Math::rrand(-15, 15);
	props.setProperty("position", v.toString());
	
	addItem(&props);
	addTeleportParticles(v);
	audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(v));
}

void bioTank3EjectHelp()
{
	Properties props;
	Vector velocity, position;
	
	position = self->position;
	position.z += self->boundingBox.maxs.z;
	
	velocity.set(Math::rrand(-15, 15), Math::rrand(-15, 15), 1.25);
	velocity.x *= 0.1;
	velocity.y *= 0.1;
	
	props.setProperty("classname", extractEnemyFromList("NME_PISTOL_BLOB|NME_GRENADE_BLOB|NME_SHOTGUN_BLOB|NME_MACHINEGUN_BLOB"));
	props.setProperty("helpless", "100");
	props.setProperty("velocity", velocity);
	props.setProperty("position", position);
	
	addEnemy(&props);
	
	audio->playSound(SND_GRENADE_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	self->thinkTime = 75;
	
	if ((--self->custom) == 0)
	{
		self->action = self->walk;
		self->thinkTime = 250;
	}
}

void bioTank3Die()
{
	Vector v;

	if (self->custom == 300)
	{
		for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
		{
			if ((boss == self) || (boss->action == bioTank3Die))
			{
				continue;
			}
			
			boss->action = bioTank3Die;
			boss->thinkTime = 0;
			boss->custom = 300;
			boss->velocity.x = Math::rrand(-100, 100);
			boss->velocity.y = Math::rrand(-100, 100);
			boss->velocity.z = 0;
		
			boss->velocity.x *= 0.01;
			boss->velocity.y *= 0.01;
		}
	}
	
	self->rotation.x += (2 * engine->getTimeDifference(TD_LOGIC));
	
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		generalMechLargeExplosion();
		
		// kill all enemies
		for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
		{
			unit->health = 0;
		}
		
		Mix_FadeOutMusic(5000);
		
		return;
	}
	
	int c = (int)self->custom;
	
	if ((c % 35) == 0)
	{
		v = self->position;
		v.x += Math::rrand(-5, 5);
		v.y += Math::rrand(-5, 5);
		v.z += Math::rrand(-5, 5);
		doExplosion(v, Math::rrand(5, 20), self);
	}
}

void bioTankPlasmaAttack()
{
	Boss *boss = (Boss*)self;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_PLASMA_RIFLE], getGunPosition(boss, 0), boss->target->position);
	
	addNormalBullet(self, &entityManager->weaponDef[WEP_PLASMA_RIFLE], getGunPosition(boss, 1), boss->target->position);
	
	audio->playSound(SND_HEAVY_PLASMA, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 15;
	
	if (((--self->custom) == 0) || (!unitCanSeeTarget(boss, boss->target, boss->position)))
	{
		self->action = self->walk;
		self->thinkTime = 250;
	}
}

void bioTank3Attack()
{
	Boss *boss = (Boss*)self;
	
	int r = 6;
	
	if (boss->getHealthPercent() <= 33)
	{
		r = 4;
	}
	
	switch (rand() % r)
	{
		case 0:
			boss->custom = Math::rrand(1, 2);
			boss->action = bioTankRocketAttack;
			break;
			
		case 1:
			boss->custom = Math::rrand(3, 5);
			boss->action = bioTankMortorAttack;
			boss->velocity.set(0, 0, 0);
			break;
			
		default:
			boss->custom = Math::rrand(8, 12);
			boss->action = bioTankPlasmaAttack;
			break;
	}
}

void bioTank3Seek()
{
	Vector position;

	Boss *boss;
	
	float percent = self->getHealthPercent();
	
	if (percent > 33)
	{
		if (entityManager->bossList.getSize() < 3)
		{
			boss = entityManager->spawnBoss();
		
			if ((rand() % 2) == 0)
			{
				bioTank1Init(boss);
			}
			else
			{
				bioTank2Init(boss);
			}
			
			switch (rand() % 4)
			{
				case 0:
					position.set(-352, 1690, 20);
					break;
					
				case 1:
					position.set(-160, 1690, 20);
					break;
					
				case 2:
					position.set(-32, 1690, 20);
					break;
					
				case 3:
					position.set(160, 1690, 20);
					break;
			}
			
			boss->position = position;
			boss->showHealthData = true;
			boss->thinkTime = 200;
			boss->custom = Math::rrand(1, 3);
			addTeleportParticles(position);
			audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(position));
		}
	}
	
	boss = (Boss*)self;
	
	if (!boss->showHealthData)
	{
		return;
	}
	
	float distance;
	
	if ((boss->target != NULL) && (boss->target->health > 0))
	{
		distance = Math::getDistance(boss->position, boss->target->position);
	}
	else
	{
		distance = bossSightTarget(boss, 2500);
		
		if (boss->target == NULL)
		{
			self->action = self->walk;
			return;
		}
	}
	
	if (distance > 650)
	{
		bioTankChaseTarget();
		return;
	}
	
	if ((boss->target != NULL) && (unitCanSeeTarget(boss, boss->target, boss->position)))
	{
		bioTankChaseTarget();
		boss->velocity = boss->velocity * 0.5; 
		boss->action = self->attack;
		boss->thinkTime = 0; 
	}
	
	int chance = (percent > 33) ? 25 : 4;
	
	if ((rand() % chance) == 0)
	{
		boss->velocity.x = boss->velocity.y = 0; 
		boss->custom = Math::rrand(2, 3);
		boss->touch = NULL;
		boss->action = bioTank3EjectHelp;
		boss->thinkTime = 150;
	}
	
	if (percent > 33)
	{
		bioTank3SpawnAIHacks();
	}
}

void bioTank3ReactToDamage(Entity *owner, float damage)
{
	Boss *boss = (Boss*)self;
	
	boss->applyDamage(damage);
	
	boss->target = owner;
	boss->action = boss->attack;
	boss->custom = Math::rrand(8, 12);
	faceLocation(boss, boss->target->position);
	
	if (boss->health <= 0)
	{
		boss->action = bioTank3Die;
		boss->walk = bioTank3Die;
		boss->thinkTime = 0;
		boss->custom = 300;
		boss->velocity.x = Math::rrand(-100, 100);
		boss->velocity.y = Math::rrand(-100, 100);
		boss->velocity.z = 0;
		
		boss->velocity.x *= 0.01;
		boss->velocity.y *= 0.01;
		
		if (!(self->flags & EF_DYING))
		{
			Unit *unit = entityManager->spawnEnemy(NME_DEAD_SENTRYGUN, 0);
			bioTankTopInit(unit, &entityManager->bossDef[3]);
			self->flags |= EF_DYING;
		}
	}
}

void bioTank3ReactToBullet(Bullet *bullet)
{
	audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
	addSparkParticle(bullet->position);
	bullet->health = 0;
}

void bioTank3Init(Boss* boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "BioMech Tank V3.0 (Beta)";
	
	boss->walk = &bioTank3Seek;
	boss->attack = &bioTank3Attack;
	boss->die = &bioTank3Die;
	boss->reactToBullet = &bioTank3ReactToBullet;
	boss->reactToDamage = &bioTank3ReactToDamage;
	boss->draw = &drawBioTank;
	
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
	
	boss->scale = 2;
	boss->boundingBox.mins.x *= boss->scale;
	boss->boundingBox.mins.y *= boss->scale;
	boss->boundingBox.mins.z *= boss->scale;
	boss->boundingBox.maxs.x *= boss->scale;
	boss->boundingBox.maxs.y *= boss->scale;
	boss->boundingBox.maxs.z *= boss->scale;

	boss->action = boss->walk;
	
	// Set up the Boss triggers
	((Trigger*)mission->triggerList.getFirstElement())->specialAction = bossBattle2Start;
}

// =========== Tank Top ==========

void bioTankTopInit(Unit *unit, BossDef *bossDef)
{
	unit->walk = NULL;
	unit->attack = NULL;
	unit->die = NULL;
	unit->draw = &drawUnit;
	
	unit->definition = bossDef;
	
	unit->touch = deadSentryGunTouch;
	unit->owner = self;
	
	unit->position = self->position;
	unit->position.z += self->boundingBox.maxs.z;
	unit->position.z += 8;
	
	unit->velocity.x = Math::rrand(-10, 10);
	unit->velocity.y = Math::rrand(-10, 10);
	unit->velocity.z = Math::rrand(15, 25);
	unit->velocity.x *= 0.1;
	unit->velocity.y *= 0.1;
	unit->velocity.z *= 0.1;
	
	unit->boundingBox.mins = bossDef->bbMins;
	unit->boundingBox.maxs = bossDef->bbMaxs;
	unit->updateBoundingBox();
	
	unit->health = unit->maxHealth = bossDef->health;
	unit->shield = unit->maxShield = bossDef->shield;
	unit->flags = unit->definition->flags;
	
	unit->thinkTime = 0;
	unit->custom = 500;
	unit->helpless = RAND_MAX;
}

// ========= Static Tanks for the Tank Bay ========

void drawStaticBioTank(bool transparent)
{
	if (transparent)
	{
		return;
	}
	
	Unit *unit = (Unit*)self;
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	glColor3f(1.0, 1.0, 1.0);
	
	glPushMatrix();
	{
		glTranslatef(unit->position.x, unit->position.y, unit->position.z);
		glRotatef(unit->rotation.x, 0.0, 0.0, 1.0);
		glRotatef(unit->rotation.y, 0.0, 1.0, 0.0);
		glBindTexture(GL_TEXTURE_2D, unit->definition->texture[0]->data);
		glCallList(unit->definition->model[0]->data);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		glTranslatef(unit->position.x, unit->position.y, unit->position.z + (8 * unit->scale));
		glRotatef(unit->rotation.x, 0.0, 0.0, 1.0);
		glRotatef(unit->rotation.y, 0.0, 1.0, 0.0);
		glBindTexture(GL_TEXTURE_2D, unit->definition->texture[1]->data);
		glCallList(unit->definition->model[1]->data);
	}
	glPopMatrix();
}

void bioTankInit(Unit *unit)
{
	unit->walk = &bioTankDoNothing;
	unit->attack = &bioTankDoNothing;
	unit->die = &bioTankDoNothing;
	unit->draw = &drawStaticBioTank;

	unit->thinkTime = RAND_MAX;
	
	unit->action = unit->walk;
}
