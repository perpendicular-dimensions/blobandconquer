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

#include "specialist.h"

void specialistSummonHelp(bool random)
{
	Vector position;
	String classname;
	Properties props;

	if (entityManager->enemyList.getSize() > 12)
	{
		generalUnitVanish(player->position, Math::rrand(1000, 1200));
		return;
	}
	
	if (random)
	{
		int chance = (int)(self->getHealthPercent());
		
		Math::limit(&chance, 10, 100);
		
		if ((rand() % chance) != 0)
		{
			return;
		}
		
		self->thinkTime = 75;
	}
	
	self->velocity = self->velocity * 0.1;
	self->custom = Math::rrand(1, 4);
	
	for (int i = 0 ; i < self->custom ; i++)
	{
		position = self->position;
		
		switch (i)
		{
			case 0:
				position.x += Math::rrand(25, 75);
				break;
				
			case 1:
				position.x -= Math::rrand(25, 75);
				break;
				
			case 2:
				position.y += Math::rrand(25, 75);
				break;
				
			case 3:
				position.y -= Math::rrand(25, 75);
				break;
		}
		
		position.z += Math::rrand(12, 36);
		
		props.setProperty("classname", extractEnemyFromList("NME_PISTOL_BLOB|NME_MACHINEGUN_BLOB|NME_SHOTGUN_BLOB|NME_GRENADE_BLOB"));
		props.setProperty("position", position);
		props.setProperty("thinkTime", Math::rrand(50, 100)); // don't attack instantly!
		addEnemy(&props);
		addTeleportParticles(position);
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(position));
	}
}

void specialistVanish()
{
	Boss *boss1, *boss2;
	
	boss1 = (Boss*)entityManager->bossList.getFirstElement();
	boss2 = (Boss*)boss1->next;
	
	if (boss2 == NULL)
	{
		self->action = self->walk;
		return;
	}
	
	if ((boss1->flags & EF_VANISHED) || (boss2->flags & EF_VANISHED))
	{
		self->action = self->walk;
		return;
	}
	
	generalUnitVanish(player->position, Math::rrand(1000, 1200));
	
	if ((rand() % 4) > 0)
	{
		specialistSummonHelp(false);
	}
}

bool specialistGetGun()
{
	Weapon *w = NULL;
	
	for (Weapon *weapon = (Weapon*)entityManager->weaponList.getFirstElement() ; weapon != NULL ; weapon = (Weapon*)weapon->next)
	{
		if (weapon->owner != NULL)
		{
			continue;
		}
		
		if ((self->name == "Blaze") && (weapon->definition->type == WEP_FLAMETHROWER))
		{
			w = weapon;
		}
		
		if ((self->name == "Frost") && (weapon->definition->type == WEP_ICE_GUN))
		{
			w = weapon;
		}
	}
	
	if (w == NULL)
	{
		return false;
	}
	
	Boss *boss = (Boss*)self;
	
	boss->item = w;
	boss->item->setOwner(boss);
	
	generalUnitVanish(w->position, 0);
	
	audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
	
	game->setInfoMessage(INFO_HINT, _("%s got his %s back!"), boss->getName(), w->definition->getName());
	
	return true;
}

void specialistStealGun()
{
	Boss *boss = (Boss*)self;
	
	float z = boss->velocity.z;
	
	boss->velocity = player->position;
	
	boss->velocity -= boss->position;
	boss->velocity.normalize();

	float speed = 1.5;
	
	boss->thinkTime = 0;
	
	if (Math::getDistance(player->position, boss->position) < 30)
	{
		speed = 1.85;
	}
		
	if ((player->position.z - boss->position.z) > 6)
	{
		z = boss->velocity.z * speed;
	} 
	
	boss->velocity.x *= speed;
	boss->velocity.y *= speed;
	boss->velocity.z = z;
	
	faceLocation(boss, boss->target->position);
}

void specialistChase()
{
	Boss *boss = (Boss*)self;
	
	float z = boss->velocity.z;
	
	boss->velocity = boss->target->position;
	
	boss->velocity.x += Math::rrand(-10, 10);
	boss->velocity.y += Math::rrand(-10, 10);
	
	boss->velocity -= boss->position;
	boss->velocity.normalize();

	float speed = Math::rrand(150, 600);
	speed /= 350;
	
	if (boss->flags & EF_ONGROUND)
	{
		if (Math::getDistance(boss->oldPosition, boss->position) < 0.1)
		{
			boss->velocity.z = 1.25;
		}
	}
	
	boss->thinkTime = Math::rrand(15, 35);
	
	boss->velocity.x *= speed;
	boss->velocity.y *= speed;
	boss->velocity.z = z;
	
	faceLocation(boss, boss->target->position);
}

void specialistPain(Boss *boss)
{
	if (boss->item != NULL)
	{
		boss->item->position = boss->position;
		boss->item->setOwner(NULL);
		throwSpawnedItem(boss->item);
		boss->item = NULL;
	}
	
	boss->velocity.x = Math::rrand(-25, 25);
	boss->velocity.y = Math::rrand(-25, 25);
	boss->velocity.z = Math::rrand(10, 25);
	
	boss->velocity = boss->velocity * 0.05;
	boss->thinkTime = Math::rrand(200, 250);
	boss->helpless = 1;
	boss->action = specialistVanish;
	
	boss->flags |= EF_BOUNCES;
}

void specialistFinishTackle()
{
	self->touch = NULL;
	self->action = self->walk;
}

void specialistTackle(Entity *other)
{
	if (other != player)
	{
		return;
	}
	
	Boss *boss = (Boss*)self;
	
	player->velocity = boss->velocity;
	player->velocity.z = 1;
	player->helpless = 100;
	
	boss->thinkTime = 75;
	boss->velocity.x = 0;
	boss->velocity.y = 0;
	boss->action = &specialistFinishTackle;
	
	audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(player->position));
	
	for (int i = 1 ; i < 3 ; i++)
	{
		if (game->weapon[i] == NULL)
		{
			continue;
		}
		
		if ((boss->name == "Blaze") && (game->weapon[i]->definition->type != WEP_FLAMETHROWER))
		{
			continue;
		}
		
		if ((boss->name == "Frost") && (game->weapon[i]->definition->type != WEP_ICE_GUN))
		{
			continue;
		}
		
		if (game->selectedWeapon == game->weapon[i])
		{
			game->selectedWeapon = game->weapon[0];
		}
		
		audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		
		game->setInfoMessage(INFO_HINT, _("%s stole %s"), boss->getName(), game->weapon[i]->definition->getName());
			
		boss->item = game->weapon[i];
		boss->item->setOwner(boss);
		game->weapon[i] = NULL;
		
		String s;
		s.setText("str_weapon_%d", i);
		textureManager->removeTexture(s.getText());
	}
}

void specialistSeek()
{
	Boss *boss = (Boss*)self;
	
	if (boss->helpless)
	{
		boss->rotation.set(0, 0, 0);
		boss->helpless = 0;
		boss->flags &= ~EF_BOUNCES;
		boss->flags &= ~EF_VANISHED;
	}
	
	if (!(boss->contentType & CONTENTS_SOLID))
	{
		generalUnitVanish(player->position, Math::rrand(100, 150));
		return;
	}
	
	float distance = bossSightTarget(boss, 1000);
	
	if (boss->target == NULL)
	{
		return;
	}
	
	if (boss->item == NULL)
	{
		if (!specialistGetGun())
		{
			boss->touch = specialistTackle;
			specialistStealGun();
		}
		return;
	}
	
	int r = rand() % 12;
	
	if (r < 6)
	{
		specialistChase();
	}
	else if (r < 8)
	{
		float speed = Math::rrand(150, 400);
		speed /= 600;
		
		float z = boss->velocity.z;
	
		boss->velocity = boss->position;
		boss->velocity.x += Math::rrand(-50, 50);
		boss->velocity.y += Math::rrand(-50, 50);
		
		faceLocation(boss, boss->velocity);
		
		boss->velocity -= boss->position;
		
		boss->velocity.normalize();
		boss->velocity.x *= speed;
		boss->velocity.y *= speed;
		boss->velocity.z = z;
		
		boss->thinkTime = Math::rrand(100, 150);
		
		if (Math::getDistance(boss->oldPosition, boss->position) < 0.1)
		{
			boss->velocity.z = 1.25;
		}
	}
	else
	{
		if (distance <= 100)
		{
			int r = rand() % 10;
			
			if (r < 4)
			{
				boss->velocity.x = boss->velocity.y = 0;
			}
			else if (r < 9)
			{
				specialistChase();
			}
			else
			{
				specialistChase();
				boss->velocity.z = 1.25;
			}
			
			boss->thinkTime = 25;
			boss->custom = Math::rrand(10, 25);
			boss->action = boss->attack;
		}
		else
		{
			specialistChase();
		}
	}
	
	specialistSummonHelp(true);
}

// ================= Frost =========================

void frostDie3()
{
	onFireBlobDie();
	
	Boss *boss = (Boss*)self;
	
	if (boss->item != NULL)
	{
		boss->item->position = boss->position;
		boss->item->setOwner(NULL);
		throwSpawnedItem(boss->item);
		boss->item = NULL;
	}
	
	if (entityManager->bossList.getSize() == 1)
	{
		for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
		{
			unit->health = 0;
		}
		
		Mix_FadeOutMusic(5000);
	}
}

void frostDie2()
{
	Vector velocity, target;
	float x, y, z;

	Boss *boss = (Boss*)self;
	
	self->flags |= EF_ONFIRE;
	
	for (int i = 0 ; i < 5 ; i++)
	{
		x = Math::rrand(-25, 25);
		y = Math::rrand(-25, 25);
		z = Math::rrand(0, 100);
		
		x *= 0.01;
		y *= 0.01;
		z *= 0.01;
		
		velocity.set(x, y, z);
		addGeneralDebris(boss->position, velocity, Math::rrand(1000, 2000), "textures/game/iceBlock.jpg", 0.5, true);
	}
	
	z = self->velocity.z;
	
	target = self->position;
	
	target.x += Math::rrand(-50, 50);
	target.y += Math::rrand(-50, 50);
	
	self->velocity = target;
	self->velocity -= self->position;
	self->velocity.normalize();
	self->velocity = self->velocity * 1.5;
	
	self->velocity.z = z;
			
	faceLocation((Unit*)self, target);
	
	self->thinkTime = Math::rrand(25, 100);
	
	if (--boss->custom <= 0)
	{
		boss->action = frostDie3;
	}
}

void frostDie1()
{
	self->custom = 20;
	self->action = frostDie2;
}

void frostAttack()
{
	Boss *boss = (Boss*)self;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	faceLocation(boss, boss->target->position);
	
	boss->velocity.x *= 0.85;
	boss->velocity.y *= 0.85;
	
	addIceShot(boss, boss->position, boss->target->position);
	boss->reload = 12;
	
	if (--boss->custom == 0)
	{
		boss->action = boss->walk;
		boss->thinkTime = 100;
	}
}

void frostReactToBullet(Bullet *bullet)
{
	Boss *boss = (Boss*)self;
	
	bullet->health = 0;
	
	if (boss->health <= 0)
	{
		return;
	}
	
	if (bullet->definition->type == WEP_FLAMETHROWER)
	{
		boss->applyDamage(5);
		
		if ((rand() % 50) == 0)
		{
			generalUnitVanish(player->position, Math::rrand(200, 300));
		}
	}
	
	if (bullet->definition->type == WEP_ICE_GUN)
	{
		Math::limit(&(boss->health += 10), 0, boss->maxHealth);
	}
	
	if (boss->health <= 0)
	{
		boss->thinkTime = 0;
		boss->action = boss->die;
	}
}

void frostReactToDamage(Entity *owner, float amount)
{
	Boss *boss = (Boss*)self;
	
	if (amount < 75)
	{
		return;
	}
	
	specialistPain(boss);
}

void frostInit(Boss* boss)
{
	BossDef *bossDef = &entityManager->bossDef[5];
	
	boss->name = "Frost";
	
	boss->walk = &specialistSeek;
	boss->attack = &frostAttack;
	boss->die = &frostDie1;
	boss->reactToBullet = &frostReactToBullet;
	boss->reactToDamage = &frostReactToDamage;
	boss->draw = &drawBoss;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	
	boss->health = boss->maxHealth = bossDef->health;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->showHealthData = true;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = boss->walk;
	
	Properties props;
	props.setProperty("classname", "WEP_ICE_GUN");
	
	boss->item = addWeapon(&props);
	boss->item->setOwner(boss);
}

// ================= Blaze =========================

void blazeDie()
{
	self->thinkTime = 0;
	self->velocity.x = self->velocity.y = 0;  
	self->action = blazeDie;
	
	if (!(self->flags & EF_FROZEN))
	{
		float frozenSize = max(self->boundingBox.maxs.x, self->boundingBox.maxs.y);
		frozenSize = max(self->boundingBox.maxs.z, frozenSize);
		frozenSize++;
			
		self->boundingBox.mins.set(-frozenSize, -frozenSize, -frozenSize);
		self->boundingBox.maxs.set(frozenSize, frozenSize, frozenSize);
			
		audio->playSound(SND_ICE_BLOCK, CH_ANY, camera->getSoundDistance(self->position));
		
		self->flags |= EF_FROZEN;
	}
}

void blazeAttack()
{
	Boss *boss = (Boss*)self;
	
	if (boss->reload > 0)
	{
		return;
	}
	
	faceLocation(boss, boss->target->position);
	
	boss->velocity.x *= 0.85;
	boss->velocity.y *= 0.85;
	
	addFlame(boss, boss->position, boss->target->position);
	boss->reload = 4;
	
	if (--boss->custom == 0)
	{
		boss->action = boss->walk;
		boss->thinkTime = 100;
	}
}

void blazeReactToBullet(Bullet *bullet)
{
	Boss *boss = (Boss*)self;
	
	bullet->health = 0;
	
	if (boss->health <= 1)
	{
		return;
	}
	
	if (bullet->definition->type == WEP_ICE_GUN)
	{
		boss->applyDamage(10);
		
		if ((rand() % 50) == 0)
		{
			generalUnitVanish(player->position, Math::rrand(200, 300));
		}
	}
	
	if (bullet->definition->type == WEP_FLAMETHROWER)
	{
		Math::limit(&(boss->health += 2), 0, boss->maxHealth);
	}
	
	if (boss->health <= 1)
	{
		boss->thinkTime = 0;
		boss->action = boss->die;
		Math::limit(&boss->health, 1, boss->maxHealth);
	}
}

void blazeReactToDamage(Entity *owner, float amount)
{
	Boss *boss = (Boss*)self;
	
	if (amount < 75)
	{
		return;
	}
	
	if ((boss->health <= 0) && (boss->flags & EF_FROZEN))
	{
		frozenBlobDie();
		
		if (boss->item != NULL)
		{
			boss->item->position = boss->position;
			boss->item->setOwner(NULL);
			throwSpawnedItem(boss->item);
			boss->item = NULL;
		}
		
		if (entityManager->bossList.getSize() == 1)
		{
			for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
			{
				unit->health = 0;
			}
		
			Mix_FadeOutMusic(5000);
		}
		
		return;
	}
	
	Math::limit(&boss->health, 1, boss->maxHealth);
	
	specialistPain(boss);
}

void blazeInit(Boss* boss)
{
	BossDef *bossDef = &entityManager->bossDef[6];
	
	boss->name = "Blaze";
	
	boss->walk = &specialistSeek;
	boss->attack = &blazeAttack;
	boss->die = &blazeDie;
	boss->reactToBullet = &blazeReactToBullet;
	boss->reactToDamage = &blazeReactToDamage;
	boss->draw = &drawBoss;
	
	boss->boundingBox.mins = bossDef->bbMins;
	boss->boundingBox.maxs = bossDef->bbMaxs;
	boss->updateBoundingBox();
	
	boss->definition = bossDef;
	
	boss->health = boss->maxHealth = bossDef->health;
	boss->shield = boss->maxShield = bossDef->shield;
	boss->flags = boss->definition->flags;
	
	boss->showHealthData = true;

	boss->thinkTime = 0;
	boss->custom = 500;

	boss->action = boss->walk;
	
	Properties props;
	props.setProperty("classname", "WEP_FLAMETHROWER");
	
	boss->item = addWeapon(&props);
	boss->item->setOwner(boss);
}
