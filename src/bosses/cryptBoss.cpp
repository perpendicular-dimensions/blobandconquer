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

#include "cryptBoss.h"

void galdovCryptReappear();
void galdovCryptSearch();

void galdovCryptFinish2()
{
	game->setMissionStatus(MISSION_COMPLETE);
	game->changingLevel = true;
	game->targetMission = mission->getNextMissionName();
	
	self->rotation.set(95, 90, 90);
	
	self->flags &= ~EF_VANISHED;
	self->flags |= EF_ONFIRE;
	self->flags &= ~EF_WEIGHTLESS;
}

void galdovCryptFinish1()
{
	fireTriggers("galdovDefeated", TRIGGER_TYPE_ANY);
	
	self->thinkTime = 100;
	self->action = &galdovCryptFinish2;
}

void galdovCryptStandUp()
{
	Boss *boss = (Boss*)self;

	boss->custom -= engine->getTimeDifference(TD_LOGIC);
	boss->helpless = 100;
	
	if (self->custom <= 0)
	{
		boss->flags |= EF_WEIGHTLESS;
		boss->action = galdovCryptReappear;
		boss->helpless = 0;
		boss->velocity.set(0, 0, 0);
		boss->rotation.set(0, 0, 0);
		addTeleportParticles(boss->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
		boss->position = entityManager->getEntityPosition("galdovBeamPoint");
		boss->custom = 100;
		boss->walk = galdovCryptSearch;
	}
}

void galdovCryptReform()
{
	Particle *particle;
	
	for (int i = 0 ; i < 15 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = self->position;
		particle->life = Math::rrand(25, 200);
		particle->size = Math::rrand(1, 3);
		
		particle->velocity.x = Math::rrand(-20, 20);
		particle->velocity.y = Math::rrand(-20, 20);
		particle->velocity.z = Math::rrand(-20, 20);
		
		particle->velocity.x /= 100;
		particle->velocity.y /= 100;
		particle->velocity.z /= 100;
		
		particle->color = GLColor::getRandomColor();
	}
	
	particle = particleManager->spawnParticle();
	particle->position = self->position;
	particle->life = 100;
	particle->size = (400 - self->custom) * 0.025;
}

void galdovCryptTakeC4Damage()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	self->velocity.x = 0;
	self->velocity.y = 0;

	if (self->getHealthPercent() <= 66)
	{
		galdovCryptReform();
	}
	
	if (self->custom > 0)
	{
		return;
	}

	for (Item *item = (Item*)entityManager->itemList.getFirstElement() ; item != NULL ; item = (Item*)item->next)
	{
		if (item->definition->type == ITM_STICKY_C4)
		{
			item->health = 0;
		}
	}
	
	generalMechLargeExplosion();
	self->flags &= ~EF_DEAD;
	
	if (self->getHealthPercent() > 66)
	{
		self->health -= (self->maxHealth * 0.04);
	}
	else
	{
		self->health -= (self->maxHealth * 0.33);
	}
	
	self->flags &= ~EF_VANISHED;
	self->flags &= ~EF_WEIGHTLESS;
	self->velocity.set(Math::rrand(-200, 200), Math::rrand(-200, 200), Math::rrand(100, 250));
	self->velocity = self->velocity * 0.01;
	self->custom = 200;
	self->action = galdovCryptStandUp;
	self->walk = galdovCryptStandUp;
	audio->playSound(SND_GALDOV_PAIN, CH_EFFECT8, camera->getSoundDistance(self->position));
	
	if (self->health <= 0)
	{
		self->action = galdovCryptFinish1;
		self->thinkTime = 300;
	}
}

void galdovCryptRequestC4()
{
	if (Math::getDistance(player->position, self->position) <= 30)
	{
		if ((!game->hasItem(_("C4"))) && (!game->cheatItems))
		{
			game->setGamePlayMessage(INFO_GENERAL, _("C4 required"));
			return;
		}
		
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to use C4"));
		
		if (game->isControl(CTRL_ACTION))
		{
			int i = game->removeItem(_("C4"));
			
			String s;
			s.setText("str_item_%d", i);
			textureManager->removeTexture(s.getText());
			
			self->custom = 300;
			self->thinkTime = 0;
			self->action = galdovCryptTakeC4Damage;
			
			Item *c4 = addStickyC4(player->position);
			c4->flags &= ~EF_WEIGHTLESS;
			
			audio->playSound(SND_C4_CLOCK, CH_EFFECT8, 128);
		}
	}
}

void galdovCryptReappear()
{
	galdovCryptReform();
	
	galdovCryptRequestC4();

	self->custom -= engine->getTimeDifference(TD_LOGIC);

	if (self->custom <= 0)
	{
		self->flags &= ~EF_VANISHED;
		handleStructureStateChange("galdovPoolCover", true);
		self->action = self->walk;
	}
}

void galdovCryptEnergyShotTouch(Entity *other)
{
	self->health = 0;
	self->touch = NULL;
	
	Particle *particle = particleManager->spawnParticle();
	particle->position = self->position;
	particle->life = 100;
	particle->size = 50;
	
	doExplosion(self->position, 25, player);
}

void galdovCryptFireEnergyShot()
{
	audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(self->position));

	Bullet *bullet = entityManager->spawnBullet(WEP_HOMING_ENERGY);
	bullet->position = player->position;
	bullet->position.x += Math::rrand(-5, 5);
	bullet->position.y += Math::rrand(-5, 5);
	bullet->position.z = 350;
	bullet->velocity.set(0, 0, -4);
	bullet->health = RAND_MAX;
	bullet->touch = galdovCryptEnergyShotTouch;
	bullet->owner = self;
	bullet->draw = drawBullet;
	
	self->thinkTime = 25;
	
	Vector end = bullet->position;
	end.z -= 500;
	
	Decal *decal = addGroundTargetMark(bullet->position, end);
	
	if (decal != NULL)
	{
		decal->texture = getRequiredTexture("textures/game/groundTarget.tga");
	}
	
	if ((--self->custom == 0) || (player->helpless))
	{
		self->custom = 400;
		self->action = galdovCryptReappear;
		self->position = entityManager->getEntityPosition("galdovBeamPoint");
	}
}

void galdovCryptFireInitialEnergyShot()
{
	audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(self->position));

	Bullet *bullet = entityManager->spawnBullet(WEP_HOMING_ENERGY);
	bullet->position = player->position;
	bullet->position.z = 350;
	bullet->velocity.set(0, 0, -2.5);
	bullet->health = RAND_MAX;
	bullet->touch = galdovCryptEnergyShotTouch;
	bullet->owner = self;
	bullet->draw = drawBullet;
	
	if ((--self->custom == 0) || (player->helpless))
	{
		self->action = self->walk;
	}
	
	Vector end = bullet->position;
	end.z -= 500;
	
	Decal *decal = addGroundTargetMark(bullet->position, end);
	
	if (decal != NULL)
	{
		decal->texture = getRequiredTexture("textures/game/groundTarget.tga");
	}
	
	self->thinkTime = 100;
	self->action = galdovCryptFireEnergyShot;
}

void galdovCryptRemoveSupport();

void galdovCryptPreEnergyShot()
{
	galdovCryptRemoveSupport();
	self->action = galdovCryptFireInitialEnergyShot;
	self->thinkTime = 200;
	self->flags |= EF_VANISHED;
	addTeleportParticles(self->position);
	audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(self->position));
	handleStructureStateChange("galdovPoolCover", false);
	audio->playSound(SND_GALDOV_LAUGH, CH_ANY, camera->getSoundDistance(self->position));
}

void galdovCryptBulletTouch(Entity *other)
{
	doExplosion(self->position, 3, self->owner);
	self->touch = NULL;
	self->health = 0;
}

void galdovCryptStun()
{
	if (!(self->contentType & CONTENTS_SOLID))
	{
		self->custom = 0;
	}

	galdovCryptRequestC4();

	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		self->flags |= EF_WEIGHTLESS;
		self->action = galdovCryptSearch;
		self->walk = galdovCryptSearch;
		self->thinkTime = 0;
		self->rotation.set(0, 0, 0);
		self->health = self->maxHealth;
	}
}

void galdovCryptPreStun()
{
	debug(("galdovCryptPreStun()\n"));

	if (self->flags & EF_WEIGHTLESS)
	{
		self->flags &= ~EF_WEIGHTLESS;
		self->custom = 800;
		self->walk = &galdovCryptStun;
		self->action = &galdovCryptStun;
		self->thinkTime = 0;
		self->rotation.set(0, 50, 0);
		
		doExplosion(self->position, 25, self);
	}
}

void galdovCryptReactToBullet(Bullet *bullet)
{
	if (self->getHealthPercent() > 66)
	{
		Boss *boss = (Boss*)self;
	
		if (bullet->definition->type != WEP_SHOTGUN)
		{
			boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
		}
		else
		{
			boss->applyDamage(1);
		}
		
		addSparkParticle(bullet->position);
		audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
		bullet->health = 0;
		bullet->touch = NULL;
		
		if (boss->getHealthPercent() <= 70)
		{
			float maxHealth = self->maxHealth * 0.7;
			boss->health = maxHealth;
			galdovCryptPreStun();
		}
		
		return;
	}
	
	bullet->owner = self;
	bullet->scale = 2;
	bullet->touch = &galdovCryptBulletTouch;
	bullet->health = RAND_MAX;
	bullet->position = self->position;
	bullet->rotation.x = -bullet->rotation.x;
	bullet->rotation.y = -bullet->rotation.y;
	bullet->rotation.z = -bullet->rotation.z;
	
	if (rand() % 2) bullet->realVelocity.x = -(bullet->realVelocity.x * 1.5);
	if (rand() % 2) bullet->realVelocity.y = -(bullet->realVelocity.y * 1.5);
	if (rand() % 2) bullet->realVelocity.z = -(bullet->realVelocity.z * 1.5);
}

void galdovCryptReactToDamage(Entity *owner, float damage)
{
	if (self->getHealthPercent() > 66)
	{
		Boss *boss = (Boss*)self;
		
		boss->applyDamage(damage / 4);
		
		if (boss->getHealthPercent() <= 70)
		{
			float maxHealth = self->maxHealth * 0.7;
			boss->health = maxHealth;
			galdovCryptPreStun();
		}
	}
}

void galdovCryptPrepareBeam();

void galdovCryptFireBeam()
{
	debug(("galdovCryptFireBeam()\n"));

	Boss *boss = (Boss*)self;

	Vector beam = boss->gunPosition[1];
	beam -= boss->position;
	beam.normalize();
	beam = beam * 2500;
	beam += boss->position;
	
	Vector p = boss->position;
	Vector v = beam;
	v -= boss->position;
	v.normalize();
	
	Particle *particle;
		
	int distance = (int)(Math::getDistance(p, beam));
	
	for (int i = 0 ; i < distance ; i += 4)
	{
		p += (v * 4);
		
		particle = particleManager->spawnParticle();
		particle->position = p;
		particle->life = 150;
		particle->size = 10;
		particle->color = ((i % 2) == 0) ? GLColor::cyan : GLColor::white;
	}
	
	bbManager->getTraceLocation(boss, boss->position, beam);
	
	audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(boss->position));
	
	if (bbManager->intersectedEntity != NULL)
	{
		debug(("Beam Hit '%s'\n", bbManager->intersectedEntity->getName()));
	
		if (bbManager->intersectedEntity->entityType == ET_WEAK_WALL)
		{
			destroyStructure(bbManager->intersectedEntity->getName());
			CutsceneCamera *camera = getCutsceneCamera("C4StructureCamera");
			
			if (camera != NULL)
			{
				camera->destination = bbManager->intersectedEntity->position;
				activateInGameCutscene("C4StructureCamera", 25);
			}
		}
		else
		{
			Particle *particle = particleManager->spawnParticle();
			particle->position = self->position;
			particle->life = 75;
			particle->size = 50;
			particle->color = GLColor::cyan;
		
			doExplosion(bbManager->intersectedEntity->position, 50, boss);
		}
	}
	else
	{
		debug(("Beam Missed\n"));
	}
	
	if (boss->numberOfBeams == 16)
	{
		game->setInfoMessage(INFO_HINT, _("DIE, BOB!!! DIE!!!!"));
	}
	
	boss->numberOfBeams--;
	
	if (boss->numberOfBeams <= 0)
	{
		boss->flags |= EF_ALWAYS_FACE;
		boss->thinkTime = 200;
		boss->custom = Math::rrand(10, 35);
		boss->action = galdovCryptPreEnergyShot;
	}
	else
	{
		boss->thinkTime = 25;
		boss->gunPosition[1] = player->position;
		boss->gunPosition[1].z = boss->position.z;
	}
}

void galdovCryptPrepareBeam()
{
	Boss *boss = (Boss*)self;
	
	if (self->custom == 500)
	{
		debug(("galdovCryptPrepareBeam()\n"));
	
		boss->position = entityManager->getEntityPosition("galdovBeamPoint");
		addTeleportParticles(boss->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
		audio->playSound(SND_GALDOV_CHARGE, CH_EFFECT8, camera->getSoundDistance(boss->position));
	}

	boss->flags &= ~EF_VANISHED;
	boss->custom -= engine->getTimeDifference(TD_LOGIC);
	boss->velocity.set(0, 0, 0);
	
	if (boss->custom <= 0)
	{
		boss->energyCharge = 0;
		boss->thinkTime = 75;
		boss->gunPosition[1] = player->position;
		boss->gunPosition[1].z = boss->position.z;
		
		boss->flags &= ~EF_ALWAYS_FACE;
		boss->action = galdovCryptFireBeam;
		
		addHalo(boss->position, 1.5);
		audio->playSound(SND_GALDOV_FIRE, CH_EFFECT8, camera->getSoundDistance(boss->position));
	}
}

void galdovCryptRemoveSupport()
{
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->definition != &entityManager->bossDef[5])
		{
			continue;
		}
	
		addTeleportParticles(unit->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(unit->position));
		unit->flags |= EF_VANISHED;
		unit->flags |= EF_DEAD;
		unit->health = 0;
	}
}

void galdovCryptSummon()
{
	Boss *boss = (Boss*)self;
	
	if (boss->energyCharge >= 15)
	{
		debug(("galdovCryptSummon - Energy >= 15\n"));
	
		galdovCryptRemoveSupport();
		handleStructureStateChange("galdovPoolCover", false);
		boss->flags |= EF_VANISHED;
		audio->playSound(SND_GALDOV_LAUGH, CH_ANY, camera->getSoundDistance(self->position));
		boss->action = &galdovCryptPrepareBeam;
		boss->velocity.set(0, 0, 0);
		addTeleportParticles(boss->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(self->position));
		boss->thinkTime = 500;
		boss->custom = 500;
		boss->numberOfBeams = 1;
		
		if (boss->getHealthPercent() <= 33)
		{
			boss->numberOfBeams = 16;
		}
		
		return;
	}
	
	// Four Dark Blobs in cutscene, plus the two
	// that we want to summon (6)
	if (entityManager->enemyList.getSize() > 6)
	{
		return;
	}
	
	Unit *unit = entityManager->spawnEnemy(NME_PISTOL_DROID, 0);
	unit->definition = &entityManager->bossDef[5];
	unit->spawnedIn = true;
	
	unit->position = self->position;
	unit->position.x += Math::rrand(-50, 50);
	unit->position.y += Math::rrand(-50, 50);
	
	addTeleportParticles(unit->position);
	audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(unit->position));
	
	blackDroidInit(unit, &entityManager->bossDef[5]);
}

void galdovMachineGunAttack()
{
	Vector targetPosition;
	
	Boss *boss = (Boss*)self;

	boss->velocity = boss->velocity * 0.9995;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	int weapon = WEP_UZI;
	
	if (boss->energyCharge >= 10)
	{
		weapon = WEP_PLASMA_RIFLE;
	}
	else if (boss->energyCharge >= 5)
	{
		weapon = WEP_PLASMA_PISTOL;
	}
	
	for (int i = 0 ; i < 2 ; i++)
	{
		targetPosition = player->position;
		targetPosition.x += Math::rrand(-5, 5);
		targetPosition.y += Math::rrand(-5, 5);
		addNormalBullet(self, &entityManager->weaponDef[weapon], self->position, targetPosition);
	}
	
	audio->playSound(entityManager->weaponDef[weapon].firingSound, CH_ANY, camera->getSoundDistance(self->position));
	
	boss->reload = 8;
	
	if ((--self->custom) == 0)
	{
		self->action = self->walk;
	}
}

void galdovPreMachineGunAttack()
{
	int min = 10;
	int max = 25;
	
	if (self->getHealthPercent() <= 33)
	{
		min = 30;
		max = 40;
	}
	else if (self->getHealthPercent() <= 66)
	{
		min = 20;
		max = 30;
	}

	self->custom = Math::rrand(min, max);
	self->action = &galdovMachineGunAttack;
}

void galdovCryptSearch()
{
	self->thinkTime = Math::rrand(2, 5);
	self->thinkTime *= 100;
	
	if ((rand() % 2) == 0)
	{
		self->velocity = player->position;
		self->velocity.z += 50;
		self->velocity -= self->position;
		self->velocity.normalize();
		
		float speed = Math::getDistance(self->position, player->position);
		
		if (speed > 800)
		{
			speed = 4;
		}
		else if (speed > 400)
		{
			speed = 2;
		}
		else
		{
			speed = 1;
		}
		
		self->velocity = self->velocity * speed;
		
		self->thinkTime = 50;
		
		self->action = galdovPreMachineGunAttack;
	}
	else
	{
		self->velocity.set(0, 0, 0);
		
		if ((rand() % 2) == 0)
		{
			self->action = galdovPreMachineGunAttack;
		}
	}
	
	if (self->getHealthPercent() <= 66)
	{	
		galdovCryptSummon();
		
		if ((rand() % 30) == 0)
		{
			if (self->getHealthPercent() <= 33)
			{
				self->custom = Math::rrand(25, 50);
			}
			else
			{
				self->custom = Math::rrand(10, 15);
			}
			self->action = galdovCryptPreEnergyShot;
			self->thinkTime = 0;
		}
	}
}

void galdovCryptStart()
{
	handleStructureStateChange("galdovPoolCover", false);
	
	self->walk = galdovCryptSearch;
	self->action = self->walk;
}

void cryptBossInit(Boss* boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "Galdov";
	boss->entityType = ET_BOSS;
	
	boss->walk = &galdovCryptStart;
	boss->attack = &galdovCryptSearch;
	boss->die = &galdovCryptSearch;
	boss->reactToBullet = galdovCryptReactToBullet;
	boss->reactToDamage = galdovCryptReactToDamage;
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
	
	boss->showHealthData = true;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = galdovCryptStart;
	
	getRequiredTexture("textures/game/groundTarget.tga");
}
