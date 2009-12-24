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

#include "bullets.h"

void assignBulletActionType(Bullet *bullet);
void addClusterBomblet(Vector position, Entity *owner);

void drawBullet(bool transparent)
{
	if (transparent)
	{
		return;
	}
	
	Bullet *bullet = (Bullet*)self;
	WeaponDef *def = (WeaponDef*)bullet->definition;

	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();
	{
		glTranslatef(bullet->position.x, bullet->position.y, bullet->position.z);
		
		glColor3f(1.0, 1.0, 1.0);
		
		glEnable(GL_TEXTURE_2D);
		
		if (bullet->texture != NULL)
		{
			glBindTexture(GL_TEXTURE_2D, bullet->texture->data);
		}
		
		switch (bullet->definition->type)
		{
			case WEP_PISTOL:
			case WEP_SHOTGUN:
			case WEP_UZI:
			case WEP_PLASMA_PISTOL:
			case WEP_PLASMA_RIFLE:
				if (bullet->owner != NULL)
				{
					if (bullet->owner->entityType == ET_BLOB)
					{
						glColor4fv(def->blobColor.color);
					}
					else if (bullet->owner->entityType == ET_BIOMECH)
					{
						glDisable(GL_TEXTURE_2D);
						
						if (def->bioMechColor.color[3] != -1)
						{
							glColor4fv(def->bioMechColor.color);
						}
						else
						{
							glColor4fv(GLColor::getRandomColor().color);
						}
					}
				}
				break;
		}
		
		if (!(def->flags & EF_STATIC))
		{
			bullet->rotation.x += (1 * engine->getTimeDifference(TD_LOGIC));
			bullet->rotation.y += (2 * engine->getTimeDifference(TD_LOGIC));
			bullet->rotation.z += (3 * engine->getTimeDifference(TD_LOGIC));
		}
		
		if ((def->type == WEP_WEB) || (def->type == WEP_ROCKET_LAUNCHER) || (def->type == WEP_MINI_ROCKET))
		{
			bullet->rotation.y += (2 * engine->getTimeDifference(TD_LOGIC));
		}
		
		glRotatef(bullet->rotation.x, 0.0, 0.0, 1.0);
		glRotatef(bullet->rotation.y, 0.0, 1.0, 0.0);
		glRotatef(bullet->rotation.z, 1.0, 0.0, 0.0);
		
		glScalef(bullet->scale, bullet->scale, bullet->scale);
		
		if (bullet->model != NULL)
		{
			glCallList(bullet->model->data);
		}
	}
	glPopMatrix();
	
	if (def->type == WEP_GRAPPLING_HOOK)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
			
		glLineWidth(2.0);
		
		glPushMatrix();
		{
			glBegin(GL_LINES);
			{
				glColor4f(1.0, 1.0, 1.0, 0.85); glVertex3f(self->position.x, self->position.y, self->position.z);
				glColor4f(0.85, 0.85, 0.85, 0.75); glVertex3f(self->owner->position.x, self->owner->position.y, self->owner->position.z);
			}
			glEnd();
		}
		glPopMatrix();
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
}

void doBulletUnitHit(Unit *unit, int damage, int damageType)
{
	Bullet *bullet = (Bullet*)self;
	
	bool wasAlive = (unit->health > 0);
	
	float shield = unit->shield;
	float maxShield = unit->maxShield;
	
	unit->applyDamage(damage, damageType);
	
	if ((unit->entityType == ET_BIOMECH) && (unit->flags & EF_FROZEN))
	{
		unit->health = -1;
	}
	
	if (unit == player)
	{
		game->alterStat(_("Total Damage Taken"), damage);
	}
	
	// shield was destroyed in previous hit!
	if ((unit->shield == -2.5) && (shield > 0))
	{
		addShieldDestroyedParticles(unit->position);
		audio->playSound(SND_SHIELD_DESTROYED, CH_ANY, camera->getSoundDistance(unit->position));
		unit->shieldHit = 0;
	}
	
	// shield was completely destroyed! (player only)
	if (unit == player)
	{
		if ((unit->maxShield == 0) && (maxShield > 0))
		{
			addShieldDestroyedParticles(unit->position);
			unit->shieldHit = 0;
			game->setInfoMessage(INFO_BAD, _("CAUTION - SHIELD BELT DESTROYED!"));
		}
	}
	
	if (bullet->owner != NULL)
	{
		if ((wasAlive) && (unit->health <= 0) && (unit->entityType == ET_BIOMECH))
		{
			switch (bullet->owner->entityType)
			{
				case ET_BLOB:
					if (bullet->owner == player)
					{
						game->alterStat(_("Enemies Killed"), 1);
					}
					else
					{
						game->alterStat(_("Enemies Killed By Teeka"), 1);
					}
					break;
				
				case ET_BIOMECH:
					game->alterStat(_("Enemies Killed By Friendly Fire"), 1);
					break;
			}
		}
	}
				
	game->alterStat(_("Bullets Hit"), 1);
		
	if (unit == player)
	{
		game->alterStat(_("Total Damage Taken"), damage);
	}
	
	// first check for combo hits
	if ((unit->health <= 0) && (unit->flags & EF_DYING))
	{
		if (!(unit->flags & EF_STATIC))
		{
			throwUnit(unit, bullet->velocity);
			unit->flags &= ~EF_DYING;
		}
		
		if (bullet->owner != NULL)
		{
			if ((bullet->owner == player) || (bullet->owner->definition->type == BLOB_TEEKA))
			{
				game->comboTimer = 150;
				game->comboHits++;
			}
		}
		
		return;
	}
	
	if (!(unit->flags & EF_FROZEN))
	{
		if ((unit->maxShield == 0) || (unit->shield <= 0))
		{
			if (!(unit->flags & EF_EXPLODES))
			{
				addBloodParticles(bullet);
				addBloodSplat(bullet->position, Math::rrand(2, 4));
				audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(bullet->position));
			}
			else
			{
				addSparkParticle(bullet->position);
				audio->playSound(SND_CLANG, CH_ANY, camera->getSoundDistance(bullet->position));
			}
		}
		else
		{
			if (unit->shield > 0)
			{
				addShieldHitParticles(bullet->position);
				audio->playSound(SND_ELECTRIC_SPARK4, CH_ANY, camera->getSoundDistance(bullet->position));
			}
		}
	}
	
	if (unit->health <= 0)
	{
		// force of impact - should be overridden in die() if needed
		if (!(unit->flags & EF_STATIC))
		{
			throwUnit(unit, bullet->velocity);
		}
		
		if (bullet->owner == player)
		{
			game->comboTimer = 150;
			game->comboHits++;
		}
	}
	else
	{
		if (!(unit->flags & EF_STATIC))
		{
			unit->velocity.x += (bullet->velocity.x * 0.05);
			unit->velocity.y += (bullet->velocity.y * 0.05);
		}
		
		unit->reload += 5;
	}
	
	if ((unit != player) && (unit->target == NULL) && (!(unit->flags & EF_STATIC)))
	{
		unit->target = (Unit*)bullet->owner;
		unit->thinkTime = 0;
	}
}

void doBulletDamage(Entity *other, int damage)
{
	if (self->health == 0)
	{
		return;
	}
	
	if ((other == self->owner) && (self->owner != NULL))
	{
		return;
	}
	
	Bullet *bullet = (Bullet*)self;
	WeaponDef *def = (WeaponDef*)bullet->definition;
	
	if ((def->type == WEP_PLASMA_PISTOL) || (def->type == WEP_PLASMA_RIFLE))
	{
		addPlasmaHitParticles(bullet->position);
		audio->playSound(SND_PLASMA_HIT, CH_ANY, camera->getSoundDistance(bullet->position));
	}
	
	if (other != NULL)
	{
		if (other->entityType == ET_BULLET)
		{
			return;
		}
		
		if ((other->entityType != ET_BIOMECH) && (bullet->owner == player))
		{
			game->alterStat(_("Bullets Missed"), 1);
		}
		
		if ((other->entityType == ET_BOSS) || (other->entityType == ET_NEUTRAL))
		{
			Entity *oldSelf = self;
			self = other;
			((Boss*)other)->reactToBullet(bullet);
			self = oldSelf;
			return;
		}
		
		bullet->health = 0;
		
		Unit *unit;
		
		switch (other->entityType)
		{
			case ET_BLOB:
			case ET_BIOMECH:
				unit = (Unit*)other;
				doBulletUnitHit(unit, damage, def->damageType);
				if (unit->maxHealth > 5)
				{
					if (unit->totalCurrentDamage >= (unit->maxHealth / 5))
					{
						if ((rand() % 5) == 0)
						{
							throwUnit(unit, bullet->velocity);
						}
					}
				}
				break;
				
			case ET_WINDOW:
				if (!(other->flags & EF_IMMORTAL))
				{
					other->health -= damage;
				}
				
				if (other->health > 0)
				{
					addSparkParticle(bullet->position);
					audio->playSound(Math::rrand(SND_GLASS_HIT_1, SND_GLASS_HIT_2), CH_ANY, camera->getSoundDistance(bullet->position));
					//addBulletHole(bullet->position, bullet->velocity);
				}
				break;
				
			case ET_SUPPLY_CRATE:
				other->health -= damage;
				addSparkParticle(bullet->position);
				break;
				
			case ET_BARRIER:
				addSparkParticle(bullet->position);
				audio->playSound(SND_ELECTRIC_SPARK4, CH_ANY, camera->getSoundDistance(bullet->position));
				break;
				
			case ET_STRUCTURE:
				if (!(other->flags & EF_IMMORTAL))
				{
					other->health -= damage;
					addSparkParticle(bullet->position);
				}
				else
				{
					addBulletHitParticles(bullet);
					audio->playSound(Math::rrand(SND_RICO1, SND_RICO2), CH_ANY, camera->getSoundDistance(bullet->position));
				}
				break;
				
			case ET_WEAK_WALL:
				addBulletHitParticles(bullet);
				addBulletMark(bullet->position, bullet->velocity);
				audio->playSound(Math::rrand(SND_RICO1, SND_RICO2), CH_ANY, camera->getSoundDistance(bullet->position));
				break;
				
			default:
				addBulletHitParticles(bullet);
				audio->playSound(Math::rrand(SND_RICO1, SND_RICO2), CH_ANY, camera->getSoundDistance(bullet->position));
				break;
		}
	}
	else
	{
		addBulletMark(bullet->position, bullet->velocity);
		
		if ((def->type != WEP_PLASMA_PISTOL) && (def->type != WEP_PLASMA_RIFLE))
		{
			addBulletHitParticles(bullet);
			audio->playSound(Math::rrand(SND_RICO1, SND_RICO2), CH_ANY, camera->getSoundDistance(bullet->position));
		}
		
		if (bullet->owner == player)
		{
			game->alterStat(_("Bullets Missed"), 1);
		}
		
		bullet->health = 0;
	}
	
	self->touch = NULL;
}

void clusterLauncherHit(Entity *other)
{
	if (self->owner == other)
	{
		return;
	}
	
	self->health = 0;
	
	for (int i = 0 ; i < 10 ; i++)
	{
		addClusterBomblet(self->position, other);
	}
	
	self->touch = NULL;
}

void rocketLauncherHit(Entity *other)
{
	if (self->owner == other)
	{
		return;
	}
	
	self->health = 0;
	self->touch = NULL;
	
	if (other != NULL)
	{
		if (other->flags & EF_IMMORTAL)
		{
			return;
		}
		
		if (other->health < 50)
		{
			// kill it instantly
			other->health = 0;
		}
		else if (other->entityType == ET_BIOMECH)
		{
			Unit *unit = (Unit*)other;
			unit->shield = unit->maxShield = 0;
			unit->health -= 25;
			
			if (!(other->flags & EF_STATIC))
			{
				unit->velocity = self->velocity;
			}
		}
	}
}

void explosiveBulletHit(Entity *other)
{
	if ((other == self->owner) && (self->owner != NULL))
	{
		return;
	}
	
	self->health = 0;
	self->touch = NULL;
}

void bulletHitSingleDamage(Entity *other)
{
	doBulletDamage(other, 1);
}

void bulletHitDamage(Entity *other)
{
	Bullet *bullet = (Bullet*)self;
	
	doBulletDamage(other, ((WeaponDef*)bullet->definition)->damage);
}

void webHit(Entity *other)
{
	self->health = 0;
	
	addPuffParticles(self->position, GLColor::white, 5, 1, 2);
	
	if (other == NULL)
	{
		audio->playSound(SND_WEB_HIT, CH_ANY, camera->getSoundDistance(self->position));
		return;
	}
	
	if ((other->entityType != ET_BLOB) && (other->entityType != ET_BIOMECH))
	{
		audio->playSound(SND_WEB_HIT, CH_ANY, camera->getSoundDistance(self->position));
		return;
	}
	
	Unit *unit = (Unit*)other;
	
	switch (unit->definition->type)
	{
		case NME_MACHINEGUN_SPIDERBLOB:
		case NME_SHOTGUN_SPIDERBLOB:
		case NME_PLASMA_SPIDERBLOB:
		case NME_BABY_SPIDERBLOB:
		case NME_UZI_SENTRY_GUN:
		case NME_PLASMA_SENTRY_GUN:
		case NME_ROCKET_SENTRY_GUN:
		case NME_MORTOR_SENTRY_GUN:
		case NME_DARK_BLOB:
			audio->playSound(SND_WEB_HIT, CH_ANY, camera->getSoundDistance(self->position));
			return;
			break;
	}
	
	if (unit->flags & EF_WRAPPED)
	{
		audio->playSound(SND_WEB_HIT, CH_ANY, camera->getSoundDistance(self->position));
		return;
	}
	
	unit->flags |= EF_WRAPPED;
	unit->flags |= EF_STATIC;
		
	unit->thinkTime = 0;
	unit->action = generalUnitWrapped;
	unit->mental = -100;
	
	self->touch = NULL;
	
	audio->playSound(SND_WEB_WRAP, CH_ANY, camera->getSoundDistance(self->position));
}

void iceShotHit(Entity *other)
{
	self->health = 0;
	
	addIceHitParticles(self->position);
	audio->playSound(SND_ICE_HIT, CH_ANY, camera->getSoundDistance(self->position));
	
	if (other == NULL)
	{
		return;
	}
	
	if ((other->entityType == ET_BOSS) || (other->entityType == ET_NEUTRAL))
	{
		Bullet *bullet = (Bullet*)self;
		Entity *oldSelf = self;
		self = other;
		((Boss*)other)->reactToBullet(bullet);
		self = oldSelf;
		return;
	}
	
	if ((other->entityType != ET_BLOB) && (other->entityType != ET_BIOMECH))
	{
		return;
	}
	
	if (other->flags & EF_FROZEN)
	{
		return;
	}
	
	Unit *unit = (Unit*)other;
	
	unit->flags &= ~EF_ONFIRE;
	
	if (other->entityType != ET_BLOB)
	{
		unit->mental = 0;
	}
	else
	{
		unit->mental -= 100;
	}
	
	if (unit->mental <= 0)
	{
		unit->flags |= EF_FROZEN;
		unit->flags |= EF_PUSHABLE;
		
		unit->thinkTime = 0;
		unit->action = generalUnitFrozen;
		
		float frozenSize = max(unit->boundingBox.maxs.x, unit->boundingBox.maxs.y);
		frozenSize = max(unit->boundingBox.maxs.z, frozenSize);
		frozenSize++;
		
		unit->boundingBox.mins.set(-frozenSize, -frozenSize, -frozenSize);
		unit->boundingBox.maxs.set(frozenSize, frozenSize, frozenSize);
		
		audio->playSound(SND_ICE_BLOCK, CH_ANY, camera->getSoundDistance(self->position));
	}
	
	self->touch = NULL;
}

void flameHit(Entity *other)
{
	if (other == NULL)
	{
		self->health = 0;
		return;
	}
	
	//self->touch = NULL;
	self->boundingBox.boxType = BOX_HOLLOW;
	
	if ((other->entityType == ET_BOSS) || (other->entityType == ET_NEUTRAL))
	{
		Bullet *bullet = (Bullet*)self;
		Entity *oldSelf = self;
		self = other;
		((Boss*)other)->reactToBullet(bullet);
		self = oldSelf;
		return;
	}
	
	if ((other->entityType != ET_BLOB) && (other->entityType != ET_BIOMECH))
	{
		return;
	}
	
	Unit *unit = (Unit*)other;
	
	if (unit->flags & EF_FROZEN)
	{
		unit->mental += 10;
		return;
	}
	
	if (!(unit->flags & EF_ONFIRE))
	{
		unit->flags |= EF_ONFIRE;
		
		if (unit == player)
		{
			game->setInfoMessage(INFO_BAD, _("YOU'RE ON FIRE!!! FIND SOME WATER!!!"));
		}
		else
		{
			unit->thinkTime = 0;
			unit->action = generalBlobFirePanic;
		}
		
		unit->helpless = 100;
		
		if (game->bloodPolicy != BLOOD_POLICY_OFF)
		{
			audio->playSound(Math::rrand(SND_DEATH1, SND_DEATH4), (unit == player) ? CH_PLAYER : CH_DEATH, camera->getSoundDistance(self->position));
		}
	}
	else if (unit != player)
	{
		unit->helpless = 5;
	}
}

void desimilatorHit(Entity *other)
{
	self->health = 0;
	
	addDesimilatorParticles(self->position);
	
	if (other == NULL)
	{
		return;
	}
	
	if ((other == NULL) || (other->entityType != ET_BIOMECH) || (other->flags & EF_FROZEN) || (other->health <= 0))
	{
		return;
	}
	
	Unit *unit = (Unit*)other;
	
	if ((!(unit->flags & EF_NOTARGET)) && (!game->cheatDesimilate))
	{
		return;
	}
	
	unit->mental -= 15;
	
	if (unit->mental <= 0)
	{
		// create a new MIA blob based on the enemy's data
		Properties props;
		
		props.setProperty("name", unit->getName());
		props.setProperty("position", unit->position.toString());
		props.setProperty("classname", "BLOB_MIA");
		props.setProperty("health", unit->health);
		props.setProperty("maxHealth", unit->maxHealth);
		props.setProperty("shield", unit->shield);
		props.setProperty("maxShield", unit->maxShield);
		props.setProperty("helpless", 100);
		props.setProperty("velocity", "0 0 0.25");
		props.setProperty("flags", EF_BOUNCES);
		
		addBlob(&props);
		
		// now remove the old enemy...
		unit->health = 0;
		unit->flags |= EF_DEAD;
		
		addDesimilatedParticles(unit->position);
		
		fireTriggers("ANY_ENEMY", TRIGGER_TYPE_ENEMY_DESIMILATED);
		
		audio->playSound(SND_DESIMILATED, CH_ANY, camera->getSoundDistance(self->position));
	}
	
	self->touch = NULL;
}

void grenadeHit(Entity *other)
{
	if (other == NULL)
	{
		if (self->realVelocity.z >= EPSILON)
		{
			audio->playSound(SND_METAL_BOUNCE, CH_ANY, camera->getSoundDistance(self->position));
		}
		
		return;
	}
	
	if (self->owner == other)
	{
		return;
	}
	
	switch (other->entityType)
	{
		case ET_DOOR:
		case ET_SWITCH:
		case ET_PUSHBLOCK:
		case ET_LIFT:
		case ET_AUTOLIFT:
			audio->playSound(SND_METAL_BOUNCE, CH_ANY, camera->getSoundDistance(self->position));
			break;
			
		case ET_STRUCTURE:
			if (other->flags & EF_IMMORTAL)
			{
				audio->playSound(SND_METAL_BOUNCE, CH_ANY, camera->getSoundDistance(self->position));
			}
			else
			{
				self->health = 0;
			}
			break;
			
		default:
			self->health = 0;
			break;
	}
	
	self->touch = NULL;
}

void bulletDieExplode()
{
	Bullet *bullet = (Bullet*)self;
	
	doExplosion(bullet->position, ((WeaponDef*)bullet->definition)->damage, bullet->owner);
}

void miniMineCheck()
{
	BoundingBox box;
	Vector mins, maxs;
	
	Bullet *bullet = (Bullet*)self;
	
	mins = self->position;
	mins.x -= 25;
	mins.y -= 25;
	
	maxs = self->position;
	maxs.x += 25;
	maxs.y += 25;
	
	int c = bbManager->initCollisionCandidates(mins, maxs);
	
	Entity *ent;
	WeaponDef *def = ((WeaponDef*)bullet->definition);
	
	for (int i = 0 ; i < c ; i++)
	{
		ent = bbManager->candidate[i];
	
		if (fabs(ent->position.z - self->position.z) > (def->damage / 2))
		{
			continue;
		}
		
		switch (ent->entityType)
		{
			case ET_BLOB:
			case ET_BIOMECH:
				self->health = 0;
				break;
				
			default:
				break;
		}
	}
	
	bbManager->clearCollisionCandidates();
	
	self->health--;
	
	if (self->health <= 0)
	{
		doExplosion(bullet->position, def->damage, bullet->owner);
	}
	
	self->thinkTime = 100;
}

void miniMineStick(Entity *other)
{
	if (other == NULL)
	{
		self->velocity.set(0, 0, 0);
		self->flags |= EF_WEIGHTLESS;
	}
	else
	{
		Bullet *bullet = (Bullet*)self;
		
		doExplosion(bullet->position, ((WeaponDef*)bullet->definition)->damage, bullet->owner);
	}
}

void grapplingHookPullOwner()
{
	Entity *owner = self->owner;
	
	self->health = RAND_MAX;
	
	if (Math::getDistance(owner->position, self->position) <= engine->getTimeDifference(TD_LOGIC) * owner->boundingBox.getSize())
	{
		owner->action = owner->walk;
		owner->thinkTime = 0;
		owner->velocity.set(0, 0, 0);
		
		owner->flags &= ~EF_WEIGHTLESS;
		owner->flags &= ~EF_GRAPPLING;
		
		self->health = 0;
		
		if (owner == player)
		{
			owner->action = NULL;
		}
	}
}

void grapplingHookPause()
{
	audio->playSound(SND_GRAPPLING, CH_ANY, 128);
	self->thinkTime = 25;
	self->action = grapplingHookPullOwner;
}

void grapplingHookCheck()
{
	Entity *owner = self->owner;
	
	if ((!(owner->flags & EF_GRAPPLING)) || (owner->health <= 0))
	{
		self->health = 0;
		return;
	}
	
	self->health = RAND_MAX;
	
	if (Math::getDistance(self->destination, self->position) <= engine->getTimeDifference(TD_LOGIC) * 8)
	{
		self->position = self->destination;
		
		addSparkParticle(self->position);
		
		audio->playSound(SND_GRAPPLE_HIT, CH_ANY, 128);
		
		owner->velocity = self->position;
		owner->velocity -= owner->position;
		owner->velocity.normalize();
		owner->velocity = owner->velocity * 3;
		
		self->action = grapplingHookPause;
		self->velocity.set(0, 0, 0);
		
		owner->flags |= EF_WEIGHTLESS;
		
		self->thinkTime = 25;
	}
}

void addNormalBullet(Entity *owner, WeaponDef *definition, Vector origin, Vector destination)
{
	destination -= origin;
	destination.normalize();
	destination.x *= BULLET_MOVE_SPEED;
	destination.y *= BULLET_MOVE_SPEED;
	destination.z *= BULLET_MOVE_SPEED;
	
	Bullet *bullet = entityManager->spawnBullet(definition->type);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 200;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
		
		if (owner->entityType == ET_BLOB)
		{
			bullet->color = GLColor::white;
		}
		else
		{
			bullet->color = GLColor::purple;
		}
	}
		
	assignBulletActionType(bullet);
}

void addWeb(Entity *owner, Vector origin, Vector destination)
{
	destination -= origin;
	destination.normalize();
	destination.x *= BULLET_MOVE_SPEED;
	destination.y *= BULLET_MOVE_SPEED;
	destination.z *= BULLET_MOVE_SPEED;
	
	Bullet *bullet = entityManager->spawnBullet(WEP_WEB);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 300;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
		bullet->color = GLColor::white;
	}
	
	assignBulletActionType(bullet);
}

void addIceShot(Entity *owner, Vector origin, Vector destination)
{
	destination -= origin;
	destination.normalize();
	destination.x *= BULLET_MOVE_SPEED;
	destination.y *= BULLET_MOVE_SPEED;
	destination.z *= BULLET_MOVE_SPEED;
	
	Bullet *bullet = entityManager->spawnBullet(WEP_ICE_GUN);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 50;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
		
		if (owner->entityType == ET_BLOB)
		{
			bullet->color = GLColor::white;
		}
		else
		{
			bullet->color = GLColor::purple;
		}
	}
	
	assignBulletActionType(bullet);
}

void addFlame(Entity *owner, Vector origin, Vector destination)
{
	destination -= origin;
	destination.normalize();
	destination.x *= (BULLET_MOVE_SPEED * 0.75);
	destination.y *= (BULLET_MOVE_SPEED * 0.75);
	destination.z *= (BULLET_MOVE_SPEED * 0.75);
	
	Bullet *bullet = entityManager->spawnBullet(WEP_FLAMETHROWER);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 50;
	
	bullet->boundingBox.mins.set(-2.5, -2.5, -2.5);
	bullet->boundingBox.maxs.set(2.5, 2.5, 2.5);
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
		
		if (owner->entityType == ET_BLOB)
		{
			bullet->color = GLColor::white;
		}
		else
		{
			bullet->color = GLColor::purple;
		}
	}
	
	assignBulletActionType(bullet);
}

void addDesimilatorBeam(Unit *owner, Vector origin, Vector destination)
{
	destination -= origin;
	destination.normalize();
	destination.x *= BULLET_MOVE_SPEED;
	destination.y *= BULLET_MOVE_SPEED;
	destination.z *= BULLET_MOVE_SPEED;
	
	Bullet *bullet = entityManager->spawnBullet(WEP_DESIMILATOR);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 35;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
	}
	
	assignBulletActionType(bullet);
	
	bullet->color = GLColor::white;
}

void addPellet(Unit *owner, WeaponDef *definition, Vector origin, Vector destination)
{
	float x = Math::rrand(-200, 200);
	float y = Math::rrand(-200, 200);
	float z = Math::rrand(-5, 5);
	
	x /= 50;
	y /= 50;
	z /= 25;
	
	destination.x += x;
	destination.y += y;
	destination.z += z;
	
	destination -= origin;
	destination.normalize();
	destination.x *= BULLET_MOVE_SPEED;
	destination.y *= BULLET_MOVE_SPEED;
	destination.z *= BULLET_MOVE_SPEED;
	
	Bullet *bullet = entityManager->spawnBullet(definition->type);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 200;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
		
		if (owner->entityType == ET_BLOB)
		{
			bullet->color = GLColor::white;
		}
		else
		{
			bullet->color = GLColor::purple;
		}
	}
	
	assignBulletActionType(bullet);
}

void throwGrenade(Unit *owner, Vector origin, Vector destination)
{
	Bullet *bullet = entityManager->spawnBullet(WEP_GRENADE);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
	}
	
	bullet->velocity = destination;
	bullet->velocity -= origin;
	
	bullet->velocity.x *= 0.0055;
	bullet->velocity.y *= 0.0055;
	bullet->velocity.z = 1.25;
	
	bullet->health = (owner == player ? 170 : 300);
	
	assignBulletActionType(bullet);
}

void addRocket(Entity *owner, Vector origin, Vector destination)
{
	destination -= origin;
	destination.normalize();
	destination.x *= BULLET_MOVE_SPEED;
	destination.y *= BULLET_MOVE_SPEED;
	destination.z *= BULLET_MOVE_SPEED;
	
	Bullet *bullet = entityManager->spawnBullet(WEP_ROCKET_LAUNCHER);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 500;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
	}
	
	assignBulletActionType(bullet);
}

void addLauncherShot(Entity *owner, WeaponDef *weaponDef, Vector origin, Vector destination)
{
	Bullet *bullet;
	
	bullet = entityManager->spawnBullet(weaponDef->type);
	bullet->setOwner(owner);
	bullet->position = origin;
	bullet->velocity = destination;
	bullet->health = 9999;
	
	if (bullet->owner != NULL)
	{
		bullet->rotation = owner->rotation;
	}
	
	bullet->velocity = destination;
	bullet->velocity -= origin;
	
	/*
		It takes about 0.8 seconds for a entity with z velocity of 1
		to return to the original point it came from...
	*/
	
	bullet->velocity.x *= 0.01;
	bullet->velocity.y *= 0.01;
	bullet->velocity.z = 1.25;
	
	assignBulletActionType(bullet);
}

void addClusterBomblet(Vector position, Entity *owner)
{
	Bullet *bullet = entityManager->spawnBullet(WEP_BOMBLET);
	bullet->setOwner(owner);
	bullet->position = position;
	bullet->health = 1000;
	
	bullet->velocity.x = Math::rrand(-50, 50);
	bullet->velocity.y = Math::rrand(-50, 50);
	bullet->velocity.z = Math::rrand(50, 100);
	
	float velocity = Math::rrand(100, 150);
	velocity *= 0.0001;
	
	bullet->velocity = bullet->velocity * velocity;
	
	assignBulletActionType(bullet);
}

void addMiniMine(Entity *owner, Vector position, Vector velocity)
{
	Bullet *bullet = entityManager->spawnBullet(WEP_MINI_MINE);
	bullet->setOwner(owner);
	bullet->position = position;
	bullet->velocity = velocity;
	bullet->health = Math::rrand(1000, 1500);
	bullet->thinkTime = 500;
	
	assignBulletActionType(bullet);
}

Bullet *addGrapplingHook(Entity *owner, Vector destination)
{
	Bullet *bullet = entityManager->spawnBullet(WEP_GRAPPLING_HOOK);
	bullet->setOwner(owner);
	bullet->position = owner->position;
	bullet->destination = destination;
	
	bullet->velocity = destination;
	bullet->velocity -= bullet->position;
	bullet->velocity.normalize();
	bullet->velocity.x *= 8;
	bullet->velocity.y *= 8;
	bullet->velocity.z *= 8;
	
	bullet->health = RAND_MAX;
	bullet->thinkTime = 0;
	
	assignBulletActionType(bullet);
	
	owner->action = entityDoNothing;
	owner->velocity.x = owner->velocity.y = 0;
	owner->flags |= EF_GRAPPLING;
	
	return bullet;
}

void assignBulletActionType(Bullet *bullet)
{
	bullet->draw = &drawBullet;
	
	switch (bullet->definition->type)
	{
		case WEP_SHOTGUN:
			bullet->touch = bulletHitSingleDamage;
			bullet->texture = textureManager->getTexture("textures/game/bullet.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/bullet.blend.raw");
			break;
		
		case WEP_GRENADE:
			bullet->touch = grenadeHit;
			bullet->die = bulletDieExplode;
			bullet->texture = textureManager->getTexture("textures/game/thrownGrenade.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/throwngrenade.blend.raw");
			break;
			
		case WEP_MINI_GRENADES:
			bullet->touch = grenadeHit;
			bullet->die = bulletDieExplode;
			bullet->texture = textureManager->getTexture("textures/game/thrownGrenade.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/throwngrenade.blend.raw");
			break;
			
		case WEP_GRENADE_LAUNCHER:
			bullet->touch = explosiveBulletHit;
			bullet->die = bulletDieExplode;
			bullet->texture = textureManager->getTexture("textures/game/thrownGrenade.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/throwngrenade.blend.raw");
			break;
			
		case WEP_CLUSTER_LAUNCHER:
			bullet->touch = clusterLauncherHit;
			bullet->die = bulletDieExplode;
			bullet->texture = textureManager->getTexture("textures/game/thrownGrenade.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/throwngrenade.blend.raw");
			break;
			
		case WEP_ROCKET_LAUNCHER:
			bullet->touch = rocketLauncherHit;
			bullet->die = bulletDieExplode;
			bullet->model = modelManager->getSimpleModel("gfx/rw2/rocket.blend.raw");
			bullet->texture = textureManager->getTexture("textures/game/rocket.png");
			break;
			
		case WEP_MINI_ROCKET:
			bullet->touch = explosiveBulletHit;
			bullet->die = bulletDieExplode;
			bullet->model = modelManager->getSimpleModel("gfx/rw2/rocket.blend.raw");
			bullet->texture = textureManager->getTexture("textures/game/rocket.png");
			break;
			
		case WEP_BOMBLET:
			bullet->touch = explosiveBulletHit;
			bullet->die = bulletDieExplode;
			bullet->texture = textureManager->getTexture("textures/game/thrownGrenade.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/throwngrenade.blend.raw");
			break;
			
		case WEP_MINI_MINE:
			bullet->action = miniMineCheck;
			bullet->touch = miniMineStick;
			bullet->die = bulletDieExplode;
			bullet->texture = textureManager->getTexture("textures/game/thrownGrenade.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/throwngrenade.blend.raw");
			break;
			
		case WEP_ICE_GUN:
			bullet->touch = iceShotHit;
			break;
			
		case WEP_FLAMETHROWER:
			bullet->touch = flameHit;
			break;
			
		case WEP_WEB:
			bullet->touch = webHit;
			bullet->texture = textureManager->getTexture("textures/game/web.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/bullet.blend.raw");
			break;
			
		case WEP_DESIMILATOR:
			bullet->touch = desimilatorHit;
			break;
			
		case WEP_GRAPPLING_HOOK:
			bullet->texture = textureManager->getTexture("textures/game/bullet.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/hookfired.blend.raw");
			bullet->action = grapplingHookCheck;
			break;
		
		default:
			bullet->texture = textureManager->getTexture("textures/game/bullet.png");
			bullet->model = modelManager->getSimpleModel("gfx/rw2/bullet.blend.raw");
			bullet->touch = bulletHitDamage;
			break;
	}
}

void addBullet(Properties *props)
{
	Bullet *bullet = entityManager->spawnBullet(WEP_PISTOL);
	
	int type = props->getInt("definitionType", 0);
	
	bullet->definition = &entityManager->weaponDef[type];
	
	bullet->load(props);
	
	bullet->setName("_bul");
	
	assignBulletActionType(bullet);
}

/*
 * Will show blood if required
*/
void doBulletHitEffect(Bullet *bullet, Unit *unit)
{
	if (!(unit->flags & EF_EXPLODES))
	{
		addBloodParticles(bullet);
	}
	else
	{
		addSparkParticle(bullet->position);
	}
	
	game->alterStat(_("Bullets Hit"), 1);
}

void doBullets()
{
	WeaponDef *def;
	bool inWater;
	
	for (Bullet *bullet = (Bullet*)entityManager->bulletList.getFirstElement() ; bullet != NULL ; bullet = (Bullet*)bullet->next)
	{
		self = bullet;
		
		//self->riding = NULL;
		
		bbManager->removeBox(bullet);
		
		if (bullet->health <= 0)
		{
			if (bullet->die != NULL)
			{
				bullet->die();
			}
			
			bullet = (Bullet*)bullet->previous;
			entityManager->bulletList.remove(bullet->next);
			continue;
		}
		
		def = (WeaponDef*)bullet->definition;
		
		if (bullet->owner != NULL)
		{
			bullet->owner->referenced = true;
		}
		
		Math::limit(&(bullet->thinkTime -= engine->getTimeDifference(TD_LOGIC)), 0, RAND_MAX);
		
		if (bullet->thinkTime == 0)
		{
			if (bullet->action != NULL)
			{
				bullet->action();
			}
		}
		
		if ((def->type == WEP_GRAPPLING_HOOK) || (withinViewableRange(bullet)))
		{
			entityManager->addEntityToDrawList(bullet);
		}
		
		bullet->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		inWater = (bullet->contentType & CONTENTS_WATER);
		
		moveEntity(bullet);
		
		bullet->health -= engine->getTimeDifference(TD_LOGIC);
		
		if ((!inWater) && (bullet->contentType & CONTENTS_WATER))
		{
			audio->playSound(SND_BULLET_HIT_WATER, CH_EFFECT1, camera->getSoundDistance(bullet->position));
		}
		
		if ((bullet->contentType & CONTENTS_WATER) && (bullet->flags & EF_WEIGHTLESS))
		{
			addBubbleParticle(bullet->position);
		}
		
		if (def->trailType != TRAIL_NONE)
		{
			addTrailParticle(bullet);
		}
		
		bbManager->addBox(bullet);
	}
}
