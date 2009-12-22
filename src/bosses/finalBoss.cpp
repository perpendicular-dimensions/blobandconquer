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

#include "finalBoss.h"

void galdovFinalWound();
void galdovFinalSearch();

Boss *energyBallFindGaldov()
{
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if (boss->name == "Galdov")
		{
			return boss;
		}
	}
	
	printf("ERROR: Energy ball cannot find Galdov\n");
	abort();
	return NULL;
}

void energyBallPreRotate()
{
	Boss *boss = (Boss*)self;
	
	boss->flags &= ~EF_VANISHED;
	boss->custom = -3.0 + (boss->energyCharge * 1.5);
	boss->speed = 0.025;
	
	boss->custom += engine->getTimeDifference(TD_LOGIC) * boss->speed;
	boss->position = energyBallFindGaldov()->position;
	boss->position.y += sin(boss->custom) * 8;
	boss->position.x += (sin(boss->custom) - cos(boss->custom)) * 8;
	boss->position.z += cos(boss->custom) * 8;
	
	addTeleportParticles(boss->position);
	audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
	
	boss->action = boss->walk;
}

void energyBallRegroup()
{
	Boss *boss = (Boss*)self->next;
	
	while (boss != NULL)
	{
		boss->flags |= EF_VANISHED;
		boss->action = energyBallPreRotate;
		boss->definition = &entityManager->bossDef[boss->energyCharge];
		boss->thinkTime = 150;
		addTeleportParticles(boss->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
		
		boss = (Boss*)boss->next;
	}
	
	self->action = self->walk;
	self->thinkTime = 100;
}

void energyBallChase();

void energyBallAttack()
{
	Boss *boss = (Boss*)self;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	faceLocation(boss, player->position);

	addNormalBullet(self, &entityManager->weaponDef[WEP_PLASMA_RIFLE], self->position, player->position);
	audio->playSound(entityManager->weaponDef[WEP_PLASMA_RIFLE].firingSound, CH_ANY, camera->getSoundDistance(self->position));
	boss->reload = entityManager->weaponDef[WEP_PLASMA_RIFLE].reloadTime;
	
	if ((--self->custom) == 0)
	{
		self->action = energyBallChase;
	}
}

void energyBallChase()
{
	if ((rand() % 3) == 0)
	{
		self->action = energyBallAttack;
		self->custom = Math::rrand(8, 18);
	}
	
	self->thinkTime = Math::rrand(100, 200);

	int r = rand() % 10;
	
	if (r <= 1)
	{
		return;
	}
	else if (r <= 3)
	{
		self->velocity.set(0, 0, 0);
		self->action = energyBallAttack;
		self->custom = Math::rrand(8, 18);
		return;
	}
	
	self->velocity = player->position;
	self->velocity.x += Math::rrand(-100, 100);
	self->velocity.y += Math::rrand(-100, 100);
	self->velocity -= self->position;
	self->velocity.normalize();
	self->velocity.z = 0;
}

void energyBallTrail()
{
	if (self->health <= 0)
	{
		addOnFireParticles();
		return;
	}

	Boss *boss = (Boss*)self;

	Particle *particle = particleManager->spawnParticle();
	particle->position = boss->position;
	particle->size = 1 * (boss->speed * 25);
	particle->life = 50;
	
	switch (boss->definition->type)
	{
		case 1:
			particle->color = GLColor::red;
			break;
			
		case 2:
			particle->color = GLColor::green;
			break;
			
		case 3:
			particle->color = GLColor::yellow;
			break;
			
		case 4:
			particle->color = GLColor::cyan;
			break;
	}
}

void energyBallDie();

void energyBallPreDie(Boss *boss)
{
	boss->walk = energyBallDie;
	boss->action = energyBallDie;
	boss->thinkTime = 0;
	boss->custom = 75;
	
	boss->velocity.x = Math::rrand(-100, 100);
	boss->velocity.y = Math::rrand(-100, 100);
	boss->velocity.z = Math::rrand(100, 200);
	boss->velocity = boss->velocity * 0.01;
	boss->flags &= ~EF_WEIGHTLESS;
}

void energyBallRotate()
{
	Boss *boss = (Boss*)self;
	
	Boss *galdov = energyBallFindGaldov();

	boss->custom += engine->getTimeDifference(TD_LOGIC) * boss->speed;
	boss->position = galdov->position;
	boss->position.y += sin(boss->custom) * 8;
	boss->position.x += (sin(boss->custom) - cos(boss->custom)) * 8;
	boss->position.z += cos(boss->custom) * 8;
	
	galdov->health += 0.25 * (engine->getTimeDifference(TD_LOGIC) * entityManager->bossList.getSize() - 1);
	Math::limit(&galdov->health, 0, galdov->maxHealth);
	
	if (boss->health <= 0)
	{
		energyBallPreDie(boss);
	}
}

bool allBallsDead()
{
	Boss *boss = (Boss*)self->next;
	
	while (boss != NULL)
	{
		if (boss->health > 0)
		{
			return false;
		}
		
		boss = (Boss*)boss->next;
	}
	
	return true;
}

void energyBallDie()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom > 0)
	{
		return;
	}

	generalMechLargeExplosion();
	self->flags |= EF_DEAD;
	
	if (allBallsDead())
	{
		Boss *boss = energyBallFindGaldov();
		boss->health = boss->maxHealth * 0.75;
		boss->action = galdovFinalWound;
	}
}

void energyBallReactToBullet(Bullet *bullet)
{
	// hit by Galdov
	if (bullet->owner == energyBallFindGaldov())
	{
		return;
	}

	addSparkParticle(bullet->position);
	audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
	bullet->health = 0;
	bullet->touch = NULL;
	
	if (self->health <= 0)
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	if (bullet->definition->type != WEP_SHOTGUN)
	{
		boss->applyDamage(((WeaponDef*)bullet->definition)->damage);
	}
	else
	{
		boss->applyDamage(1);
	}
	
	if (boss->health <= 0)
	{
		energyBallPreDie(boss);
	}
}

void energyBallReactToDamage(Entity *owner, float damage)
{
	if (owner != player)
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	if (boss->health <= 0)
	{
		return;
	}

	// energy balls don't take full damage
	boss->applyDamage(damage / 4);
	
	if (boss->health <= 0)
	{
		energyBallPreDie(boss);
	}
}

void energyBallInit(Boss *boss, int i)
{
	BossDef *bossDef = &entityManager->bossDef[i];
	
	boss->entityType = ET_BOSS;
	boss->energyCharge = i;
	boss->speed = 0.025;
	
	boss->walk = &energyBallRotate;
	boss->attack = &energyBallRotate;
	boss->die = &energyBallRotate;
	boss->reactToBullet = energyBallReactToBullet;
	boss->reactToDamage = energyBallReactToDamage;
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

	boss->thinkTime = 0;

	boss->action = energyBallRotate;
	
	boss->showHealthData = true;
	
	boss->bossEffect = energyBallTrail;
}

// ================ Energy Shot ===============

void energyShotReactToBullet(Bullet *bullet)
{
	bullet->health = 0;
}

void energyShotReactToDamage(Entity *owner, float damage)
{
	if ((damage >= 75) && (owner == player))
	{
		self->velocity.x = -self->velocity.x * 2;
		self->velocity.y = -self->velocity.y * 2;
		self->velocity.z = -self->velocity.z * 2;
		self->owner = player;
	}
}

void energyShotDie()
{
	self->thinkTime = Math::rrand(5, 10);

	self->custom--;
	
	if (self->custom == 0)
	{
		self->flags |= EF_DEAD;
	
		if ((self->owner != player) && (rand() % 2))
		{
			showMessageDialog(_("A strong blast might be able to send that shot back from where it came..."), "OK");
		}
		
		return;
	}
	
	audio->playSound(SND_GRENADE_EXPLODE, CH_EXPLODE, camera->getSoundDistance(self->position));
	
	Particle *particle;
	
	for (int i = 0 ; i < 25 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->position = self->position;
		particle->position.x += Math::rrand(-10, 10);
		particle->position.y += Math::rrand(-10, 10);
		particle->position.z += Math::rrand(-10, 10);
		particle->size = Math::rrand(5, 10);
		particle->life = 100;
		
		particle->velocity.x = Math::rrand(-20, 20);
		particle->velocity.y = Math::rrand(-20, 20);
		particle->velocity.z = Math::rrand(-20, 20);
		
		particle->velocity = particle->velocity * 0.01;
		
		switch (rand() % 4)
		{
			case 0:
				particle->color = GLColor::white;
				break;
				
			case 1:
				particle->color = GLColor::blue;
				break;
				
			case 2:
				particle->color = GLColor::cyan;
				break;
				
			case 3:
				particle->color = GLColor::purple;
				break;
		}
	}
}

void energyShotTouch(Entity *other)
{
	if (other == self->owner)
	{
		return;
	}
	
	bool kill = false;

	if (other == NULL)
	{
		kill = true;
	}
	else if ((other->entityType == ET_BOSS) && (other->definition->type == 0))
	{
		Boss *boss = (Boss*)other;
		boss->health -= boss->maxHealth * 0.25;
		boss->action = galdovFinalWound;
		boss->thinkTime = 0;
		
		kill = true;
	}
	else if (other == player)
	{
		Unit *unit = (Unit*)other;
		unit->applyDamage(25, DAMAGE_SPECIAL); // instant kill...
		
		kill = true;
	}
	
	if (kill)
	{
		self->health = 0;
		self->touch = NULL;
		self->flags |= EF_VANISHED;
		self->walk = energyShotDie;
		self->action = energyShotDie;
		self->realVelocity.set(0, 0, 0);
		self->custom = 10;
	}
}

void energyShotDoNothing()
{
}

void energyShotEffect()
{
	Particle *particle = particleManager->spawnParticle();
	particle->position = self->position;
	particle->size = Math::rrand(5, 10);
	particle->life = 50;
	
	switch (rand() % 4)
	{
		case 0:
			particle->color = GLColor::white;
			break;
			
		case 1:
			particle->color = GLColor::blue;
			break;
			
		case 2:
			particle->color = GLColor::cyan;
			break;
			
		case 3:
			particle->color = GLColor::purple;
			break;
	}
}

void fireEnergyShot(Boss *galdov)
{
	//
	// Sideshow Bob: No, that's German for "The, Bart, The."
	// Jury: No one who speaks German could be an evil man!
	//
	game->setInfoMessage(INFO_HINT, _("DIE, BOB!!! DIE!!!!"));

	galdov->energyCharge = 0;

	Boss *boss = entityManager->spawnBoss();
	boss->owner = galdov;
	
	BossDef *bossDef = &entityManager->bossDef[6];
	
	boss->name = "Shot";
	boss->entityType = ET_BOSS;
	
	boss->walk = &energyShotDoNothing;
	boss->attack = &energyShotDoNothing;
	boss->die = &energyShotDoNothing;
	boss->touch = &energyShotTouch;
	boss->reactToBullet = energyShotReactToBullet;
	boss->reactToDamage = energyShotReactToDamage;
	boss->draw = &drawBoss;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	boss->gunPosition[0] = bossDef->gunPosition[0];
	boss->gunPosition[1] = bossDef->gunPosition[1];
	
	boss->velocity = player->position;
	boss->velocity -= galdov->position;
	boss->velocity.normalize();
	boss->velocity = boss->velocity * 5;
	boss->position = galdov->position;
	
	boss->health = boss->maxHealth = bossDef->health;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->bossEffect = energyShotEffect;
	
	boss->showHealthData = false;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = energyShotDoNothing;
}

// ================ Galdov ====================

void galdovFinalFinish()
{
	game->setMissionStatus(MISSION_COMPLETE);
	game->changingLevel = true;
}

void galdovFinalStandUp()
{
	Boss *boss = (Boss*)self;

	boss->custom -= engine->getTimeDifference(TD_LOGIC);
	boss->helpless = 100;
	
	if (self->custom <= 0)
	{
		boss->flags |= EF_WEIGHTLESS;
		boss->helpless = 0;
		boss->velocity.set(0, 0, 0);
		boss->rotation.set(0, 0, 0);
		boss->action = boss->walk;
		boss->thinkTime = 200;
		addTeleportParticles(boss->position);
		
		String s;
		s.setText("galdovChargePoint%d", 1 + rand() % 2);
		boss->position = entityManager->getEntityPosition(s.getText());
		
		addTeleportParticles(boss->position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(boss->position));
	}
}

void galdovFinalWound()
{
	generalMechLargeExplosion();
	self->flags &= ~EF_DEAD;
	self->flags &= ~EF_WEIGHTLESS;
	self->velocity.set(Math::rrand(-200, 200), Math::rrand(-200, 200), Math::rrand(100, 250));
	self->velocity = self->velocity * 0.01;
	self->custom = 200;
	self->action = galdovFinalStandUp;
	audio->playSound(SND_GALDOV_PAIN, CH_EFFECT8, camera->getSoundDistance(self->position));
	
	if (self->health <= 0)
	{
		self->action = galdovFinalFinish;
		self->thinkTime = 300;
	}
}

void galdovFinalSummon()
{
	Vector position;
	String classname;
	Properties props;

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
		
		if (self->getHealthPercent() > 50)
		{
			if (entityManager->bossList.getSize() > 1)
			{
				props.setProperty("classname", extractEnemyFromList("NME_PISTOL_BLOB"));
			}
			else
			{
				props.setProperty("classname", extractEnemyFromList("NME_LIGHT_PLASMA_BLOB"));
			}
			
			props.setProperty("position", position);
			props.setProperty("thinkTime", Math::rrand(100, 300)); // don't attack instantly!
			addEnemy(&props);
			addTeleportParticles(position);
			audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(position));
		}
		else
		{
			Unit *unit = entityManager->spawnEnemy(NME_PISTOL_DROID, 0);
			unit->definition = &entityManager->bossDef[5];
			
			unit->position = self->position;
			unit->position.x += Math::rrand(-50, 50);
			unit->position.y += Math::rrand(-50, 50);
			
			addTeleportParticles(unit->position);
			audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(unit->position));
			
			blackDroidInit(unit, &entityManager->bossDef[5]);
			
			unit->die = &blackDroidReleaseAndDie;
			unit->health = unit->maxHealth = unit->maxHealth * 3.5; // not as easy to kill as Crypt version
		}
	}
}

void galdovFinalResetBalls()
{
	Boss *boss = (Boss*)self->next;
	
	while (boss != NULL)
	{
		boss->speed -= (engine->getTimeDifference(TD_LOGIC) * 0.00025);
		
		if (boss->speed <= 0.025)
		{
			boss->definition = &entityManager->bossDef[boss->energyCharge];
			boss->action = energyBallRotate;
			boss->speed = 0.025;
			self->thinkTime = 150;
			self->action = self->walk;
		}
		
		boss = (Boss*)boss->next;
	}
}

void galdovFinalSplitBallsAttack()
{
	if ((rand() % 10) == 0)
	{
		self->action = energyBallRegroup;
		self->walk = &galdovFinalSearch;
		return;
	}
	
	self->velocity = player->position;
	self->velocity -= self->position;
	self->velocity.normalize();
	self->velocity.z = 0;
	self->thinkTime = 100;
	
	if ((rand() % 2) == 0)
	{
		self->thinkTime = 50;
		self->action = galdovPreMachineGunAttack;
	}
}

void galdovFinalSplitBalls()
{
	Boss *boss = (Boss*)self;
	
	boss->walk = &galdovFinalSplitBallsAttack;
	boss->action = &galdovFinalSplitBallsAttack;
	
	boss = (Boss*)self->next;
	
	while (boss != NULL)
	{
		boss->action = energyBallChase;
		boss->thinkTime = 0;
		boss = (Boss*)boss->next;
	}
}

void galdovFinalClusterAttack()
{
	Boss *boss = (Boss*)self;

	boss->custom--;
	boss->thinkTime = 10;
	
	Bullet *bullet = entityManager->spawnBullet(WEP_GRENADE_LAUNCHER);
	
	bullet->position = boss->position;
	bullet->setOwner(boss);
	bullet->health = 300;
	bullet->velocity.x = Math::rrand(-20, 20);
	bullet->velocity.y = Math::rrand(-20, 20);
	bullet->velocity = bullet->velocity * 0.1;
	bullet->velocity.z = Math::rrand(1, 2);
	
	assignBulletActionType(bullet);
	
	audio->playSound(SND_GRENADE_LAUNCHER, CH_ANY, camera->getSoundDistance(self->position));
	
	if (boss->custom <= 0)
	{
		boss->action = &galdovFinalResetBalls;
		return;
	}
}

void galdovFinalBeam(Boss *boss, GLColor c, Vector position)
{
	Vector v = position;
	v -= boss->position;
	v.normalize();
	
	Vector p = boss->position;
	Particle *particle;
	
	int distance = (int)(Math::getDistance(boss->position, position));
	
	for (int i = 0 ; i < distance ; i += 2)
	{
		p += (v * 5);
		
		particle = particleManager->spawnParticle();
		particle->position = p;
		particle->life = Math::rrand(90, 120);
		particle->size = 2;
		particle->color = ((i % 2) == 0) ? c : GLColor::white;
	}
}

void galdovFinalBeamDecal()
{
	Vector start = player->position;
	Vector end = player->position;
	
	end.z -= 10;

	Decal *decal = addGroundTargetMark(start, end);
	
	if (decal != NULL)
	{
		decal->texture = getRequiredTexture("textures/game/groundTarget.tga");
	}
}

void galdovFinalEnergyBeamAttack()
{
	Boss *galdov = (Boss*)self;
	Boss *boss = (Boss*)self;
	
	while (boss != NULL)
	{
		galdovFinalBeam(boss, GLColor::cyan, galdov->gunPosition[1]);
		boss = (Boss*)boss->next;
	}
	
	audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(galdov->position));
	
	doExplosion(galdov->gunPosition[1], 10, galdov);
	
	Particle *particle = particleManager->spawnParticle();
	particle->position = galdov->gunPosition[1];
	particle->life = 100;
	particle->size = 50;
	
	galdov->thinkTime = 35;
	galdov->gunPosition[1] = player->position;
	galdov->gunPosition[1].z -= 4;
	
	if ((--galdov->custom == 0) || (player->health <= 0))
	{
		galdov->action = &galdovFinalResetBalls;
		return;
	}
	
	galdovFinalBeamDecal();
}

void galdovFinalShatterAttack()
{
	Boss *boss = (Boss*)self;
	
	while (boss != NULL)
	{
		galdovFinalBeam(boss, GLColor::yellow, player->position);
		boss = (Boss*)boss->next;
	}
	
	Particle *particle = particleManager->spawnParticle();
	particle->position = player->position;
	particle->life = 100;
	particle->size = 50;

	player->velocity.x = Math::rrand(-250, 250);
	player->velocity.y = Math::rrand(-250, 250);
	player->velocity.z = Math::rrand(100, 150);
	player->velocity = player->velocity * 0.01;
	
	player->helpless = 100;
	player->flags |= EF_BOUNCES;
	
	for (int i = 1 ; i < 4 ; i++)
	{
		if (game->weapon[i] == NULL)
		{
			continue;
		}
		
		if ((rand() % 8) == 0)
		{
			continue;
		}
	
		game->weapon[i]->position = player->position;
		game->weapon[i]->custom = 200;
		game->weapon[i]->setOwner(NULL);
		game->weapon[i]->inCollisionCandidateList = false;
		
		game->weapon[i]->velocity.x = Math::rrand(-65, 65);
		game->weapon[i]->velocity.y = Math::rrand(-65, 65);
		game->weapon[i]->velocity.z = Math::rrand(10, 25);
		
		game->weapon[i]->velocity = game->weapon[i]->velocity * 0.1;
		
		game->weapon[i] = NULL;
	}
	
	String s;
	s.setText("str_weapon_%d", 3);
	textureManager->removeTexture(s.getText());
	textureManager->removeTexture("str_current_ammo");
	
	game->selectedWeapon = game->weapon[0];
	
	addShieldDestroyedParticles(player->position);
	addShieldDestroyedParticles(player->position);
	
	audio->playSound(SND_LASER, CH_ANY, 128);
	audio->playSound(SND_GRENADE_EXPLODE, CH_ANY, 128);

	self->action = &galdovFinalResetBalls;
}

void galdovFinalUseBalls()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);

	if (self->custom > 0)
	{
		return;
	}

	Boss *boss;
	
	boss = (Boss*)self->next;
	
	// was just destroyed!!!
	if (boss == NULL)
	{
		self->action = self->walk;
		return;
	}
	
	int type = boss->definition->type;
	
	boss = (Boss*)self;
	
	switch (type)
	{
		// Split and Attack
		case 1:
			boss->action = &galdovFinalSplitBalls;
			break;
			
		// Scatter Attack
		case 2:
			boss->custom = 20 * entityManager->bossList.getSize();
			boss->action = galdovFinalClusterAttack;
			break;
			
		// Shatter Attack
		case 3:
			boss->action = galdovFinalShatterAttack;
			break;
			
		// Beam attack
		case 4:
			boss->action = &galdovFinalEnergyBeamAttack;
			boss->custom = (entityManager->bossList.getSize() - 1) * 3;
			boss->thinkTime = 100;
			boss->gunPosition[1] = player->position;
			boss->gunPosition[1].z -= 4;
			galdovFinalBeamDecal();
			break;
			
		default:
			printf("galdovFinalUseBalls - %d not defined\n", boss->definition->type);
			exit(1);
			break;
	}
}

bool galdovFinalCanActivatePower(int i)
{
	Boss *boss = (Boss*)self->next;
	
	while (boss != NULL)
	{
		if ((boss->definition->type == i) && (boss->health >= 0))
		{
			return true;
		}
		
		boss = (Boss*)boss->next;
	}
	
	return false;
}

void galdovFinalActivateBalls()
{
	debug(("galdovFinalActivateBalls()\n"));

	if (entityManager->bossList.getSize() == 1)
	{
		self->action = self->walk;
		return;
	}

	int i = Math::rrand(1, 4);
	
	if (!galdovFinalCanActivatePower(i))
	{
		return;
	}
	
	self->velocity.set(0, 0, 0);
	
	Boss *boss = (Boss*)self->next;
	
	while (boss != NULL)
	{
		boss->definition = &entityManager->bossDef[i];
		boss->speed = 0.1;
		boss = (Boss*)boss->next;
	}
	
	self->custom = 300;
	self->action = galdovFinalUseBalls;
}

void galdovFinalChargeShotWarn();

void galdovFinalFireChargeShot()
{
	fireEnergyShot((Boss*)self);
	
	self->action = galdovFinalChargeShotWarn;
	
	self->thinkTime = 250;
	
	self->custom--;
	
	if (self->custom <= 0)
	{
		self->thinkTime = 500;
		self->action = self->walk;
	}
}

void galdovFinalChargeShotWarn()
{
	addHalo(self->position, 1.5);
	audio->playSound(SND_GALDOV_FIRE, CH_EFFECT8, camera->getSoundDistance(self->position));
	self->thinkTime = 75;
	self->action = galdovFinalFireChargeShot;
}

void galdovFinalChargeShot()
{
	if (self->custom == 600)
	{
		addTeleportParticles(self->position);
		audio->playSound(SND_GALDOV_LAUGH, CH_ANY, camera->getSoundDistance(self->position));
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(self->position));
		audio->playSound(SND_GALDOV_CHARGE, CH_EFFECT8, camera->getSoundDistance(self->position));
		self->flags &= ~EF_VANISHED;
	}

	Particle *particle = particleManager->spawnParticle();
	particle->type = PT_SPARK;
	particle->life = 100;
	particle->size = Math::rrand(1, 2);
	
	particle->position = self->position;
	particle->position.x += Math::rrand(-10, 10);
	particle->position.y += Math::rrand(-10, 10);
	particle->position.z += Math::rrand(-10, 10);
	
	particle->velocity = self->position;
	particle->velocity -= particle->position;
	particle->velocity.normalize();
	particle->velocity = particle->velocity * 0.1;
	
	particle->color = GLColor::getRandomColor();

	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom <= 0)
	{
		self->custom = 2;
		self->action = galdovFinalChargeShotWarn;
		self->thinkTime = 0;
	}
}

void galdovFinalSearch()
{
	int r = rand() % 10;
	
	if (r <= 1)
	{
		self->thinkTime = Math::rrand(1, 2);
		self->thinkTime *= 100;
		self->velocity.set(0, 0, 0);
		
		if ((rand() % 2) == 0)
		{
			self->thinkTime = 50;
			self->action = galdovPreMachineGunAttack;
		}
		
		return;
	}
	else if (r <= 3)
	{
		self->velocity = player->position;
		self->velocity -= self->position;
		self->velocity.normalize();
		self->velocity.z = 0;
		self->thinkTime = 100;
		
		if ((rand() % 2) == 0)
		{
			self->thinkTime = 50;
			self->action = galdovPreMachineGunAttack;
		}
	}
	else if (r <= 6)
	{
		if (entityManager->bossList.getSize() != 1)
		{
			self->action = galdovFinalActivateBalls;
		}
		else
		{
			if (((Boss*)self)->energyCharge >= 20)
			{
				galdovCryptRemoveSupport();
			
				self->action = galdovFinalChargeShot;
				self->thinkTime = 300;
				self->flags |= EF_VANISHED;
				self->custom = 600;
				self->velocity.set(0, 0, 0);
				addTeleportParticles(self->position);
				
				Boss *boss = (Boss*)self->next;
		
				while (boss != NULL)
				{
					boss->flags |= EF_VANISHED;
					addTeleportParticles(boss->position);
					boss = (Boss*)boss->next;
				}
				
				String s;
				s.setText("galdovChargePoint%d", 1 + rand() % 2);
				self->position = entityManager->getEntityPosition(s.getText());
				audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(self->position));
			}
			else
			{
				if (entityManager->enemyList.getSize() < 4)
				{
					self->custom = 1;
					galdovFinalSummon();
				}
			}
		}
	}
	else
	{
		if (entityManager->enemyList.getSize() == 0)
		{
			self->custom = Math::rrand(2, 4);
			galdovFinalSummon();
		}
	}
}

void galdovFinalReactToBullet(Bullet *bullet)
{
	addSparkParticle(bullet->position);
	audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
	bullet->health = 0;
	bullet->touch = NULL;
	
	Boss *boss = (Boss*)self;
	
	if (boss->getHealthPercent() <= 50)
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
	
	if (boss->getHealthPercent() <= 50)
	{
		boss->health = boss->maxHealth * 0.5;
		boss->action = galdovFinalWound;
		boss->thinkTime = 0;
	}
}

void galdovFinalReactToDamage(Entity *owner, float damage)
{
	if (owner != player)
	{
		return;
	}

	Boss *boss = (Boss*)self;
	
	if (boss->getHealthPercent() <= 50)
	{
		return;
	}
	
	boss->applyDamage(damage);
	
	if (boss->getHealthPercent() <= 50)
	{
		boss->health = boss->maxHealth * 0.5;
		boss->action = galdovFinalWound;
		boss->thinkTime = 0;
	}
}

void finalBossInit(Boss *boss)
{
	BossDef *bossDef = &entityManager->bossDef[0];
	
	boss->name = "Galdov";
	boss->entityType = ET_BOSS;
	
	boss->walk = &galdovFinalSearch;
	boss->attack = &galdovFinalSearch;
	boss->die = &galdovFinalSearch;
	boss->reactToBullet = galdovFinalReactToBullet;
	boss->reactToDamage = galdovFinalReactToDamage;
	boss->draw = &drawBoss;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	boss->gunPosition[0] = bossDef->gunPosition[0];
	boss->gunPosition[1] = bossDef->gunPosition[1];
	
	boss->health = boss->maxHealth = bossDef->health * 7;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->showHealthData = true;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = galdovFinalSearch;
	
	#if 0
	boss->health = 1;
	boss->flags = 0;
	return;
	#endif
	
	Boss *ball;
	
	ball = entityManager->spawnBoss();
	ball->autoSaveToken = "";
	ball->position = boss->position;
	ball->custom = 0;
	ball->name = "Conquest";
	energyBallInit(ball, 1);
	
	ball = entityManager->spawnBoss();
	ball->autoSaveToken = "";
	ball->position = boss->position;
	ball->custom = 1.5;
	ball->name = "War";
	energyBallInit(ball, 2);
	
	ball = entityManager->spawnBoss();
	ball->autoSaveToken = "";
	ball->position = boss->position;
	ball->custom = 3;
	ball->name = "Famine";
	energyBallInit(ball, 3);
	
	ball = entityManager->spawnBoss();
	ball->autoSaveToken = "";
	ball->position = boss->position;
	ball->custom = 4.5;
	ball->name = "Death";
	energyBallInit(ball, 4);
	
	getRequiredTexture("textures/game/groundTarget.tga");
	
	for (Entity *ent = (Entity*)entityManager->blobList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = entityDoNothing;
		ent->thinkTime = 0;
	}
}
